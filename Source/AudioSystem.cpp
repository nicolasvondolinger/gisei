#include "AudioSystem.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include <filesystem>

SoundHandle SoundHandle::Invalid;


AudioSystem::AudioSystem(int numChannels)
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(numChannels);
    mChannels.resize(numChannels);
}


AudioSystem::~AudioSystem()
{
    for (auto& sound : mSounds)
    {
        Mix_FreeChunk(sound.second);
    }
    mSounds.clear();

    Mix_CloseAudio();
}


void AudioSystem::Update(float deltaTime)
{
    for(int i = 0; i < mChannels.size(); i++)
    {
        if(mChannels[i].IsValid())
        {
            if(!Mix_Playing(i)) {
                mHandleMap.erase(mChannels[i]);
                mChannels[i].Reset();
            }
        }
    }
}


SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping)
{
    Mix_Chunk *sound = GetSound(soundName);

    if (sound == nullptr) {
        SDL_Log("[AudioSystem] PlaySound couldn't find sound for %s", soundName.c_str());
        return SoundHandle::Invalid;
    }

    return mLastHandle;
}


void AudioSystem::StopSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] StopSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    Mix_HaltChannel(mHandleMap[sound].mChannel);
    mHandleMap.erase(sound);
    mChannels[mHandleMap[sound].mChannel].Reset();
}


void AudioSystem::PauseSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] PauseSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    if(!mHandleMap[sound].mIsPaused)
    {
        Mix_Pause(mHandleMap[sound].mChannel);
        mHandleMap[sound].mIsPaused = true;
    }
}


void AudioSystem::ResumeSound(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        SDL_Log("[AudioSystem] ResumeSound couldn't find handle %s", sound.GetDebugStr());
        return;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        Mix_Resume(mHandleMap[sound].mChannel);
        mHandleMap[sound].mIsPaused = false;
    }
}


SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
    if(mHandleMap.find(sound) == mHandleMap.end())
    {
        return SoundState::Stopped;
    }

    if(mHandleMap[sound].mIsPaused)
    {
        return SoundState::Paused;
    }

    return SoundState::Playing;
}


void AudioSystem::StopAllSounds()
{
    Mix_HaltChannel(-1);

    for(auto & mChannel : mChannels)
    {
        mChannel.Reset();
    }

    mHandleMap.clear();
}


void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
    std::error_code ec{};
    for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
    {
        std::string extension = rootDirEntry.path().extension().string();
        if (extension == ".ogg" || extension == ".wav")
        {
            std::string fileName = rootDirEntry.path().stem().string();
            fileName += extension;
            CacheSound(fileName);
        }
    }
#endif
}





void AudioSystem::CacheSound(const std::string& soundName)
{
    GetSound(soundName);
}







Mix_Chunk *AudioSystem::GetSound(const std::string& soundName)
{
    std::string fileName = "../Assets/Sounds/";
    fileName += soundName;

    Mix_Chunk* chunk = nullptr;
    auto iter = mSounds.find(fileName);
    if (iter != mSounds.end())
    {
        chunk = iter->second;
    }
    else
    {
        chunk = Mix_LoadWAV(fileName.c_str());
        if (!chunk)
        {
            SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
            return nullptr;
        }

        mSounds.emplace(fileName, chunk);
    }
    return chunk;
}


void AudioSystem::ProcessInput(const Uint8* keyState)
{
    
    if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
    {
        SDL_Log("[AudioSystem] Active Sounds:");
        for (size_t i = 0; i < mChannels.size(); i++)
        {
            if (mChannels[i].IsValid())
            {
                auto iter = mHandleMap.find(mChannels[i]);
                if (iter != mHandleMap.end())
                {
                    HandleInfo& hi = iter->second;
                    SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
                            static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
                            hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
                }
                else
                {
                    SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
                            mChannels[i].GetDebugStr());
                }
            }
        }
    }

    mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}
