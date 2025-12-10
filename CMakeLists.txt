cmake_minimum_required(VERSION 3.25.1)

# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "gisei")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/Components/Drawing/SpriteComponent.h
        Source/Components/Drawing/SpriteComponent.cpp
        Source/UI/UIButton.cpp
        Source/UI/UIButton.h
        Source/UI/UIElement.cpp
        Source/UI/UIElement.h
        Source/UI/UIImage.cpp
        Source/UI/UIImage.h
        Source/UI/AudioSystem.cpp
        Source/UI/AudioSystem.h
        Source/UI/Screens/UIScreen.cpp
        Source/UI/Screens/UIScreen.h
        Source/UI/UIText.cpp
        Source/UI/UIText.h
        Source/UI/Screens/HUD.cpp
        Source/UI/UIRect.cpp
        Source/UI/UIRect.h
        Source/UI/Screens/MainMenu.cpp
        Source/UI/Screens/MainMenu.h
        Source/UI/Screens/GameOver.cpp
        Source/UI/Screens/GameOver.h
        Source/UI/Screens/PauseMenu.cpp 
        Source/UI/Screens/PauseMenu.h
        Source/UI/Screens/IntroCrawl.cpp
        Source/UI/Screens/IntroCrawl.h
        Source/UI/Screens/ControlsMenu.cpp
        Source/UI/Screens/ControlsMenu.h
        Source/LanguageManager.cpp
        Source/LanguageManager.h
        Source/Renderer/Font.h
        Source/Renderer/Font.cpp
        Source/Actors/Yokai.cpp
        Source/UI/Screens/SettingsMenu.cpp
        Source/Actors/ParallaxBackground.cpp
        Source/Components/Drawing/ParallaxComponent.cpp
        Source/Components/Drawing/DashAfterimageComponent.cpp
        Source/Actors/Ninja.cpp
        Source/Actors/KarasuTengu.cpp
        Source/Actors/SkeletonWarrior.cpp
        Source/Actors/SkeletonSpearman.cpp
        Source/Actors/SkeletonArcher.cpp
        Source/Actors/Dart.cpp
        Source/Components/Drawing/DashSmokeComponent.cpp
        Source/Actors/SkeletonWarrior.cpp
        Source/Actors/SkeletonWarrior.h
        Source/Actors/SkeletonSpearman.cpp
        Source/Actors/SkeletonSpearman.h
        Source/Actors/SkeletonArcher.cpp
        Source/Actors/SkeletonArcher.h
        Source/Actors/Arrow.cpp
        Source/Actors/Arrow.h
        Source/Actors/KarasuTengu.cpp
        Source/Actors/KarasuTengu.h
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
    GLEW::GLEW
        SDL2::SDL2main
    SDL2::SDL2 
    SDL2_image::SDL2_image 
    SDL2_mixer::SDL2_mixer 
    SDL2_ttf::SDL2_ttf
    OpenGL::GL
)
