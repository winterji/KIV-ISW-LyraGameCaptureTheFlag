# 01 — Instalace a první spuštění

Tento dokument vás provede cestou od *"právě jsem si stáhl repozitář"* k *"hraji CTF a vidím, jak boti hrají proti sobě"*.

## 1. Instalace Unreal Engine 5.7

Stáhněte UE 5.7 přes Epic Games Launcher. Novější verze 5.x mohou fungovat, ale Lyra byla autorská proti 5.7 a špatná verze enginu je nejčastější příčina selhání instalace.

## 2. Stažení Lyra Starter Game

Lyra je referenční projekt zdarma od Epicu. V **Epic Games Launcher → Unreal Engine → Library → Marketplace** najděte *Lyra Starter Game* a klikněte **Create Project**. Připravte si disk s alespoň 30 GB volnými.

## 3. Umístění pluginu ShooterCTF

Plugin musí být v cestě:

```
<VášProjekt>/Plugins/GameFeatures/ShooterCTF/
```

Pokud složka `Plugins/GameFeatures/` neexistuje, vytvořte ji.

## 4. Aktivace pluginu

1. Otevřete projekt v Unreal Editoru.
2. **Edit → Plugins**.
3. V kategorii **Game Features** najděte **ShooterCTF** a aktivujte ho.
4. Restartujte editor, když k tomu budete vyzváni.

Po restartu by editor měl hlásit, že Game Feature `ShooterCTF` je **Registered** a její experience je načtená.

## 5. Spuštění CTF zápasu

Nejrychlejší cesta:

1. V Content Browseru jděte na `Plugins/ShooterCTF Content/Maps/`.
2. Otevřete `L_ShooterCFT_FiringRange` (menší cvičná mapa) nebo `L_Limitation` (plná CTF aréna).
3. V toolbaru klikněte **Play**.

Měla by se vám spawnout postava a po obou stranách týmů objevit boti (`B_ISW_AI` a stock Lyra boti). Vlajkové základny jsou svítící podstavce v barvách týmů na opačných koncích mapy.

## 6. Ověření, že AI žije

Během Play:

1. Otevřete **Window → Developer Tools → Output Log**. Filtrujte podle `LogBehaviorTree` pro aktivitu BT, nebo `LogAIModule` pro události controlleru.
2. Stiskněte v PIE backtick (**`**), abyste otevřeli konzoli, a napište `showdebug AI` — vykreslí AI debug overlay.
3. Stiskněte apostrof (`'`) pro **Gameplay Debugger**. Čísla 1–9 přepínají kategorie — `2` je AI, `3` je Behavior Tree, `5` je Perception. Klikněte na bota a uvidíte jeho blackboard živě.

Pokud gameplay debugger ukazuje naplněný blackboard a boti se pohybují, pronásledují a skórují, máte hotovo.

## 7. Kde jsou soubory, které budete potřebovat

| Asset                        | Cesta                                                  | K čemu slouží                          |
| ---------------------------- | ------------------------------------------------------ | -------------------------------------- |
| `B_ISW_AI`                   | `Content/Bot/B_ISW_AI`                                 | Blueprint AI controller                |
| `BB_ISW_Bot`                 | `Content/Bot/BB_ISW_Bot`                               | Blackboard                             |
| `BT_ISW_CTF_bot`             | `Content/Bot/BT_ISW_CTF_bot`                           | Hlavní Behavior Tree                   |
| `BTS_CheckLOS`               | `Content/Bot/BTS_CheckLOS`                             | Service aktualizující `CanSeeFlagCarrier` |
| `B_GrantFlagPad`             | `Content/Blueprint/B_GrantFlagPad`                     | Aktor podstavce vlajky                 |
| `B_FlagActor`                | `Content/Items/Flag/`                                  | Samotná vlajka                         |
| `B_CaptureTheFlagScoring`    | `Content/Blueprint/B_CaptureTheFlagScoring`            | Skórovací komponenta                   |
| Experience asset             | `Content/Experiences/B_ShooterGame_CaptureTheFlag`     | Lyra Experience, která zavádí CTF mód  |

## 8. Co když něco nefunguje

- **Plugin se neobjeví v okně Plugins.** Zkontrolujte, že soubor `.uplugin` je přímo v `Plugins/GameFeatures/ShooterCTF/`, ne o úroveň níž.
- **Editor padá při aktivaci.** Pravděpodobně máte špatnou verzi enginu. Potřebujete verzi 5.5+ (testováno na verzi 5.7.4)
- **Boti se objeví, ale nic nedělají.** Controller možná selhal při spouštění BT. Otevřete gameplay debugger (`'`) a koukněte na BB bota — pokud je prázdný, čtěte sekci o životním cyklu v [03 — Architektura AI](03-AI-Architecture.md).
