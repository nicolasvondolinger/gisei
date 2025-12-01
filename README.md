# 1. Screenshots

---

# 2. Título
**Gisei: The Shattered Blade**

---

# 3. Descrição
**Gisei** é um action-platformer 2D sombrio que acompanha **Kenshi**, um guerreiro artificial forjado em bambu e aço e animado por um fragmento de alma. Criado por monges devotos que o enviam ao **Pináculo Celestial** com a promessa de ascensão divina, ele acredita estar cumprindo uma prova sagrada para recuperar suas memórias e alcançar sua verdadeira forma. No entanto, fragmentos de ecos espirituais e visões de peregrinos anteriores sugerem que há algo profundamente errado com essa jornada — ninguém que trilhou esse caminho antes retornou.

No gameplay, o jogador atravessa uma fase construída via CSV com **plataformas, parallax, zoom 2x**, inimigos e espinhos, combinando **travessia precisa**, **combate**, **tiros sincronizados**, **defesa**, e principalmente **dashes curtos com invencibilidade e teleporte lateral**. As mecânicas se integram a um fluxo contínuo de jogo que começa com uma **intro narrativa** em PT/EN (com narração, vento e partículas), seguida por menus completos — Main, Pause, Settings, Controls — todos com **troca dinâmica de idioma**, efeitos sonoros de UI e ajustes de volume, fullscreen e dificuldade. Morrer por dano letal ou queda leva ao **Game Over**, enquanto alcançar o fim da fase aciona o **Stage Clear**.

A cada passo, Kenshi percebe que sua jornada não é uma ascensão, mas uma preparação. Os sussurros revelam que os “Ara-Hitogami” como ele são apenas **colheitas**, recipientes criados para amadurecer a alma até que esteja pronta para ser sacrificada a um deus adormecido. A ambientação, a intro e o design da fase reforçam esse tom trágico, conduzindo o jogador por uma narrativa de descoberta e ruptura, onde Kenshi deve decidir entre aceitar seu destino ou quebrar o ciclo que o forjou.

---

# 4. Funcionalidades a serem testadas

- **Menus:**  
  Navegação completa e sons de UI (hover/confirm/back) em Main, Pause, Settings, Controls, Game Over. Verificar troca de idioma PT/EN funcionando após rebuild das telas.

- **Intro:**  
  Texto em rolagem, vento/partículas, narração PT/EN de acordo com o idioma escolhido. Testar salto da intro com **SPACE** sem cortes abruptos.

- **Gameplay:**  
  - Movimento lateral (A/D)  
  - Pulo (Space)  
  - Ataque (J)  
  - Dash (K) com invencibilidade e teleporte curto (~200 px)  
  - Tiro (L) sincronizado com a animação  
  - Defesa/agachar (S), reduzindo velocidade e bloqueando dano  
  - Colisões com espinhos e inimigos  
  - Câmera com zoom e parallax

- **Fluxos gerais:**  
  Pausar e retornar ao jogo; Stage Clear; Game Over; carregamento correto do Level1 após a intro.

- **Áudio:**  
  Trilha do Level1, SFX de ataque/pulo/morte, narração PT/EN, sons de UI. Verificar se respeitam os ajustes de volume no menu.

- **Performance/estabilidade:**  
  Troca de idioma sem travamentos; menus abrindo e fechando sem bugs; carregamento suave da fase.

- **Menu de Pause (ESC):**  
  Verificar abertura e fechamento corretos ao apertar **ESC**, navegação entre as opções do menu de pause, funcionamento dos botões (Resume, Settings, Controls, Quit), aplicação de sons de UI (hover/confirm/back) e retorno ao jogo mantendo o estado anterior sem bugs.


---

# 5. Créditos

- **Bernardo do Nascimento Nunes** — intro (crawl, partículas/vento, narração PT/EN com seleção de idioma), sistema de idiomas (strings dinâmicas e rebuild de UI), sons de UI (hover/confirm/back) e organização de áudio, ajustes de fluxos de menu/pause/intro/level, revisão e documentação (README e instruções de playtest).

- **Lucas Cassio Costa** — Estruturação da arquitetura base do projeto, implementação das classes fundamentais (core) e integração e gerenciamento dos recursos (assets) iniciais, ajuda criação do nível 1, definição dos assets, criação de UI base (menu de início e pause).

- **Nicolas Von Dolinger** — criação do repositório, estruturação base do README, ajuda na criação do nível 1, definição de assets base, fix de animações do personagem principal(dash, agachar, defender), criação das barras de vida e energia, fix na UI, criação do menu de morte.

- **Igor Eduardo Martins Braga** - Criação dos mapas usando Tiled, criação dos inimigos e lógica de dano e ataque jogador/inimigo, estruturação do menu de pause e página de game over, criação e estruturação do primeiro boss do jogo, adição de transição entre o primeiro mapa e mapa do primeiro boss, implementação da IA dos inimigos do jogo, correção da lógica de fullscreen do jogo e câmera do jogador.

---

# SEÇÕES ADICIONAIS DO PROJETO

# História

## O Protagonista: O "Ara-Hitogami"

(Gisei, 犠牲, significa "sacrifício" em japonês)

Kenshi é uma alma despertada em um corpo artificial de bambu, aço e cordas sagradas, animado por um fragmento espiritual. Sem memória de sua vida passada, acredita que deve peregrinar até o **Pináculo Celestial** para se tornar um deus vivo. Convencido pelos monges do Templo do Silêncio, ele vê essa jornada como sua única esperança de plenitude.

## A Jornada: O Caminho do Peregrino

O mundo está corrompido por uma praga espiritual que enlouquece homens e feras. Kenshi atravessa florestas assombradas por Yōkai, montanhas guardadas por monges guerreiros e vilarejos devastados.  
Ao derrotar grandes guardiões, ele recupera memórias e habilidades:

- **O Salto do Tengu:** salto duplo/planar  
- **A Fúria do Oni:** ataque destrutivo que quebra paredes  
- **O Passo do Kitsune:** teletransporte curto (dash especial)

Rumores ecoam pelo caminho: “Nenhuma alma retorna do céu”.

## A Verdade: O Destino da "Colheita"

O Pináculo Celestial revela-se uma forja divina alimentada por almas. Os “Ara-Hitogami” são guerreiros ressuscitados criados apenas para amadurecer sua alma e serem sacrificados a um deus adormecido. Kenshi descobre os restos de outros receptáculos destruídos.

## Os Finais

### **Final 1: Revolta — A Lâmina Quebrada se Volta Contra os Deuses**
Kenshi destrói a forja e o deus, mas se sacrifica no processo. A praga cessa, o mundo renasce, mas seu nome se perde.

### **Final 2: Aceitação — A Honra no Sacrifício**
Ele se entrega à forja. O deus é apaziguado, a praga cessa… até que desperte novamente. O ciclo continua. Kenshi é esquecido.

---

# Assets

## Artes
- https://opengameart.org/  
- Exemplos:  
  - https://opengameart.org/content/a-blocky-dungeon  
  - https://opengameart.org/content/dungeon-tileset  
  - https://opengameart.org/content/a-platformer-in-the-forest  
- Gerador de personagens:  
  https://liberatedpixelcup.github.io/Universal-LPC-Spritesheet-Character-Generator/

## Músicas
- https://opengameart.org/art-search-advanced?keys=&field_art_type_tid%5B%5D=12  
- https://freemusicarchive.org/home  
- https://incompetech.com/music/royalty-free/music.html

## Efeitos Sonoros
- https://opengameart.org/art-search-advanced?keys=&field_art_type_tid%5B%5D=13

---
