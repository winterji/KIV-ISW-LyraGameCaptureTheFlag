# ShooterCTF — studentská dokumentace

> English version: [docs/README.md](../README.md) — *Anglická verze této dokumentace.*

Vítejte. Tato dokumentace je určena studentům předmětu **Inteligentní agenti**, kteří budou v rámci semestrální práce implementovat AI agenty pro herní mód `ShooterCTF`. Učebnicí předmětu je **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno, 2004), a terminologie použitá v této dokumentaci důsledně vychází z této knihy. Pokud sekce přímo odpovídá konkrétní kapitole knihy, citujeme ji ve tvaru `[Kubík 2004, §X.Y]`.

Herní mód je aréna typu Capture the Flag postavená nad projektem [Lyra Starter Game](https://www.unrealengine.com/marketplace/en-US/product/lyra) v Unreal Engine 5.7. Dodává se s baseline Blueprint AI (`B_ISW_AI`), kterého si můžete přečíst, rozšířit, nebo nahradit. Smyslem je dát vám reálné herní prostředí, ve kterém musí agent **vnímat svět, rozhodovat se a jednat** — aniž byste museli sami stavět celou enginovou infrastrukturu.

V terminologii Kubíka je baseline `B_ISW_AI` **reaktivním agentem s prvky uvažování** (reaktivní agent s symbolickou bází představ — Blackboardem — ale bez explicitního plánování). Vaše práce zpravidla bude posouvat tohoto agenta směrem k více deliberativní nebo hybridní architektuře ve smyslu [Kubík 2004, §2 a §3.2].

## Jak číst tuto dokumentaci

Pokud se s projektem setkáváte poprvé, čtěte v pořadí:

| #   | Dokument                                                              | Co obsahuje                                                                                                              |
| --- | --------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| 01  | [Instalace a první spuštění](01-Setup.md)                             | Instalace Lyry, aktivace pluginu, spuštění CTF experience.                                                               |
| 02  | [Herní mód](02-Game-Mode.md)                                          | Pravidla CTF, klíčové aktéry, průběh hry, skórování.                                                                     |
| 03  | [Architektura AI](03-AI-Architecture.md)                              | Mapování Kubíkova konceptu agenta na stack Lyra AI. Životní cyklus controlleru. Jak vypadá smyčka *vjem → rozhodnutí → akce* v Blueprintu. |
| 04  | [Blackboard a Behavior Tree](04-Blackboard-and-BehaviorTree.md)       | Každý BB klíč, kdo ho zapisuje a kdo čte. Procházka existujícím `BT_ISW_CTF_bot`.                                        |
| 05  | [Příklad — listener eliminace](05-Example-Elimination-Listener.md)    | Kompletní rozbor listeneru herní zprávy, který aktualizuje představy agenta.                                             |
| 06  | [Rozšiřování AI](06-Extending-the-AI.md)                              | Jak přidat BB klíč, BT service, BT task. Časté chyby a jak se jim vyhnout.                                               |
| 07  | [Zadání projektů](07-Assignments.md)                                  | Návrhy semestrálních projektů různé obtížnosti, každý napojený na konkrétní kapitolu Kubíka.                            |

## Co budete potřebovat

- **Unreal Engine 5.7** (verze, proti které byla Lyra postavena).
- **Lyra Starter Game** s tímto pluginem v `Plugins/GameFeatures/ShooterCTF/`.
- Základní orientaci v **Blueprint visual scriptingu**. Žádné znalosti C++ nejsou potřeba.
- Knihu **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno 2004, ISBN 80-251-0323-4). Celá pojmová síť, kterou v dokumentaci používáme (vjem / akce / vnitřní stav, reaktivní / deliberativní / sociální / hybridní agent, subsumpční architektura, BDI, stigmergie, kontraktační síť, …), pochází z této knihy. Citujeme ji jako `[Kubík 2004, §X.Y]`.

## Co je už hotové

- Fungující CTF herní mód s vyzvedáváním vlajky, jejím upuštěním a skórováním.
- Baseline Blueprint AI (`B_ISW_AI`), který:
  - na startu zjistí, kde jsou základny obou týmů;
  - pronásleduje a střílí nositele nepřátelské vlajky s tím, že střelba je podmíněna přímou viditelností;
  - reaguje na herní zprávy o vyzvednutí / doručení vlajky a o eliminaci postavy;
  - při své smrti uvolní vlajku, pokud ji zrovna nesl (viz [05](05-Example-Elimination-Listener.md)).

Záměrně **není silným protihráčem**. Smyslem je, abyste ho rozebrali a udělali lepšího.

## Co za vás neuděláme

Vaším úkolem bude **navrhnout a implementovat alespoň jedno netriviální chování AI** nad tímto baseline. Dokumentace vám říká, kde je co, co je bezpečné měnit, a na jaké přešlapy minulé generace studentů narazila — ale **architektura agenta je vaše věc**. Inspiraci najdete v [Zadání projektů](07-Assignments.md).

## Konvence použité v dokumentaci

- Cesty k souborům a assetům jsou uváděny relativně ke kořeni pluginu, např. `Content/Bot/B_ISW_AI`.
- "BB" = Blackboard. "BT" = Behavior Tree. "BTS" = BT Service. "BTT" = BT Task.
- Blueprint uzly jsou označovány stejným jménem, jaké mají v kontextovém menu editoru (anglicky, protože UE5 nepřekládá názvy uzlů).
- Citace Kubíka uvádíme ve tvaru `[Kubík 2004, §X.Y]`, kde `§X.Y` je číslo sekce v knize.
