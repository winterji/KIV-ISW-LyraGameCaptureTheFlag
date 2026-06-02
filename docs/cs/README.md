# ShooterCTF — studentská dokumentace

> English version: [docs/README.md](../README.md) — *Anglická verze této dokumentace.*

Vítejte. Tato dokumentace je určena studentům předmětu **KIV/ISW**, kteří budou v rámci semestrální práce implementovat AI agenty pro herní mód `Capture the Flag`. Dokumentace čerpá z knihy **Kubík, A. — *Inteligentní agenti*** (Computer Press, Brno, 2004) a terminologie použitá v této dokumentaci vychází z právě z této knihy.

Herní mód je aréna typu Capture the Flag postavená nad projektem [Lyra Starter Game](https://www.unrealengine.com/marketplace/en-US/product/lyra) v Unreal Engine 5.7. Dodává se s baseline Blueprint AI (`B_ISW_AI`), od kterého **budete dědit** a stavět na něm vlastního agenta. Smyslem je dát vám reálné herní prostředí, ve kterém musí agent **vnímat svět, rozhodovat se a jednat** — aniž byste museli sami stavět celou enginovou infrastrukturu.

V terminologii Kubíka je baseline `B_ISW_AI` **BDI-inspirovaným reaktivním agentem**: má explicitní bázi představ (Blackboard), implicitní cíle zakódované v dekorátorech BT a implicitní záměry jako aktuálně aktivní podstrom. Kanonickou BDI architekturu z Kubíka v plné podobě (knihovna plánů, explicitní deliberativní cyklus `options() → filter()`, prvotřídní data pro touhy a záměry) ale neimplementuje — celou diskusi viz [03 — Architektura AI, §3.3.2](03-AI-Architecture.md#332-je-b_isw_ai-bdi). Vaše práce zpravidla bude posouvat tohoto agenta směrem k více deliberativní nebo hybridní architektuře.

## Jak číst tuto dokumentaci

Pokud se s projektem setkáváte poprvé, čtěte v pořadí:

| #   | Dokument                                                              | Co obsahuje                                                                                                              |
| --- | --------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| 01  | [Instalace a první spuštění](01-Setup.md)                             | Instalace Lyry, aktivace pluginu, spuštění CTF experience.                                                               |
| 02  | [Herní mód](02-Game-Mode.md)                                          | Pravidla CTF, klíčové aktéry, průběh hry, skórování.                                                                     |
| 03  | [Architektura AI](03-AI-Architecture.md)                              | Mapování Kubíkova konceptu agenta na stack Lyra AI. Životní cyklus controlleru. Jak vypadá smyčka *vjem → rozhodnutí → akce* v Blueprintu. |
| 04  | [Blackboard a Behavior Tree](04-Blackboard-and-BehaviorTree.md)       | Každý BB klíč, kdo ho zapisuje a kdo čte. Procházka existujícím `BT_ISW_CTF_bot`.                                        |
| 05  | [Příklad — listener eliminace](05-Example-Elimination-Listener.md)    | Kompletní rozbor listeneru herní zprávy, který aktualizuje představy agenta.                                             |
| 06  | [Rozšiřování AI](06-Extending-the-AI.md)                              | Jak vytvořit vlastního agenta děděním od `B_ISW_AI`. Přidávání BB klíčů, BT service, BT task, listenerů zpráv a kooperativního chování. Časté chyby.            |
| 07  | [Zadání projektů](07-Assignments.md)                                  | Návrhy semestrálních projektů různé obtížnosti, každý napojený na konkrétní kapitolu Kubíka.                            |

## Co budete potřebovat

- **Unreal Engine 5.7** (verze, proti které byla Lyra postavena).
- **Lyra Starter Game** s tímto pluginem v `Plugins/GameFeatures/ShooterCTF/`.
- Základní orientaci v **Blueprint visual scriptingu**. Žádné znalosti C++ nejsou potřeba.
- Knihu **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno 2004, ISBN 80-251-0323-4). Celá pojmová síť, kterou v dokumentaci používáme (vjem / akce / vnitřní stav, reaktivní / deliberativní / sociální / hybridní agent, subsumpční architektura, BDI, stigmergie, kontraktační síť, …), pochází z této knihy.

## Co je už hotové

- Fungující CTF herní mód s vyzvedáváním a donášením vlajky, a skórováním.
- Baseline Blueprint AI (`B_ISW_AI`), který:
  - na startu zjistí, kde jsou základny obou týmů;
  - pronásleduje a střílí nositele nepřátelské vlajky s tím, že střelba je podmíněna přímou viditelností;
  - reaguje na herní zprávy o vyzvednutí / doručení vlajky a o eliminaci postavy;
  - při své smrti se vlajka vrátí na základnu, pokud ji zrovna nesl (viz [05](05-Example-Elimination-Listener.md)).

Vlastního agenta budete stavět **děděním od `B_ISW_AI`** — váš child Blueprint si zachová veškeré toto zapojení a přidá vlastní nebo rozšíří existující Behavior Tree a případné přidá další listenery. Baseline záměrně **není silným protihráčem** — smyslem je udělat lepšího.

## Co za vás neuděláme

Vaším úkolem bude **navrhnout a implementovat alespoň jedno netriviální chování AI** nad tímto baseline. Dokumentace vám popíše v jakém stavu je AI controller teď a jak přistoupit k jeho rozšíření. Doporučuji si opravdu přečíst alespoň [AI Architekturu](03-AI-Architecture.md) a [Rozšíření AI](06-Extending-the-AI.md) kde jsou vysvětleny důležité pojmy a postupy spolu s častými chybami. Zadání s pár tipy a doporučením najdete v [Zadání projektů](07-Assignments.md).

## Další dostupné materiály

| Popis | Url |
| ----- | ---- |
| Dokumentace Lyra Sample Game | https://dev.epicgames.com/documentation/unreal-engine/lyra-sample-game-in-unreal-engine
| YT - popis default AI controlleru v Lyra | https://www.youtube.com/watch?v=jZFgTEGRJxg
| UE5 docs - AI controller | https://dev.epicgames.com/documentation/unreal-engine/behavior-tree-in-unreal-engine---quick-start-guide

## Konvence použité v dokumentaci

- Cesty k souborům a assetům jsou uváděny relativně ke kořeni pluginu, např. `Content/Bot/B_ISW_AI`.
- "BB" = Blackboard. "BT" = Behavior Tree. "BTS" = BT Service. "BTT" = BT Task.
- Blueprint uzly jsou označovány stejným jménem, jaké mají v kontextovém menu editoru (anglicky, protože UE5 nepřekládá názvy uzlů).
- Citace Kubíka uvádíme ve tvaru `[Kubík 2004, Kapitola X.Y]`, kde `Kapitola X.Y` je číslo kapitoly v knize.
