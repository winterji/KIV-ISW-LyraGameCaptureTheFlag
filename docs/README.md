# ShooterCTF — Student Documentation

> Czech version: [docs/cs/](cs/README.md) — *Česká verze této dokumentace.*

Welcome. This documentation is for students of the **Inteligentní agenti** course who will implement AI agents inside the `ShooterCTF` game mode as a semestral project. The course textbook is **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno, 2004), and the terminology used throughout these documents is the one used in the book. Where a section maps clearly onto a chapter of the book, we cite it as e.g. `[Kubík 2004, §1.2]`.

The game mode is a Capture the Flag arena built on top of [Lyra Starter Game](https://www.unrealengine.com/marketplace/en-US/product/lyra) in Unreal Engine 5.7. It ships with a baseline Blueprint AI (`B_ISW_AI`) that you can read, extend, or replace. The point is to give you a real game environment in which an agent must perceive the world, decide, and act — without you having to build the engine plumbing yourself.

In Kubík's terminology, the baseline `B_ISW_AI` is a **reactive agent with extensions toward deliberation** (reaktivní agent s prvky uvažování) — it maintains a symbolic belief base (the Blackboard) and reacts to it through a Behavior Tree, but does not currently plan ahead. Your work will typically push it toward a more deliberative or hybrid architecture in the sense of [Kubík 2004, §2 and §3.2].

## How to read this documentation

If this is your first time touching the project, read them in order:

| #   | Document                                                          | What it covers                                                                                                  |
| --- | ----------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| 01  | [Setup](01-Setup.md)                                              | Install Lyra, enable the plugin, launch the CTF experience.                                                     |
| 02  | [Game Mode](02-Game-Mode.md)                                      | CTF rules, the actors involved, gameplay flow, scoring.                                                         |
| 03  | [AI Architecture](03-AI-Architecture.md)                          | Mapping Kubík's agent concepts onto the Lyra AI stack. Controller lifecycle. What "perception → decision → action" looks like in Blueprint. |
| 04  | [Blackboard & Behavior Tree](04-Blackboard-and-BehaviorTree.md)   | Every blackboard key, what writes it, what reads it. Walkthrough of the existing `BT_ISW_CTF_bot`.              |
| 05  | [Example — Elimination Listener](05-Example-Elimination-Listener.md) | A complete worked example of a gameplay-message listener that updates the agent's beliefs.                   |
| 06  | [Extending the AI](06-Extending-the-AI.md)                        | How to add a blackboard key, a BT service, a BT task. Common pitfalls and how to avoid them.                    |
| 07  | [Assignments](07-Assignments.md)                                  | Suggested project ideas at varying difficulty levels.                                                           |

## What you will need

- **Unreal Engine 5.7** (the version Lyra was built against)
- **Lyra Starter Game** project, with this plugin under `Plugins/GameFeatures/ShooterCTF/`
- Basic comfort with **Blueprint visual scripting**. No C++ knowledge is required for any of the assignments.
- A copy of **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno 2004, ISBN 80-251-0323-4). The whole conceptual vocabulary used throughout these docs (vjem / akce / vnitřní stav, reaktivní / deliberativní / sociální / hybridní agent, subsumpční architektura, BDI, stigmergie, kontraktační síť, …) comes from this book. We cite sections as `[Kubík 2004, §X.Y]`.

## What's already built

- A working CTF game mode with flag pickup, drop, and capture scoring.
- A baseline Blueprint AI (`B_ISW_AI`) that:
  - learns where its own and the enemy flag bases are on spawn,
  - chases and shoots an enemy flag carrier with line-of-sight gating,
  - reacts to flag pickup / delivery / elimination gameplay messages,
  - drops the flag when it dies (via the elimination listener — see doc 05).

It is intentionally **not** a strong opponent. You are meant to take it apart and do better.

## What we won't do for you

You will design and implement at least one non-trivial AI behavior on top of this baseline. The documentation tells you where things are, what's safe to change, and what gotchas other students have hit — but the agent design is yours. See [Assignments](07-Assignments.md) for ideas.

## Conventions used in this documentation

- File and asset paths are written relative to the plugin root, e.g. `Content/Bot/B_ISW_AI`.
- "BB" = Blackboard. "BT" = Behavior Tree. "BTS" = BT Service. "BTT" = BT Task.
- Blueprint nodes are shown with the same name they have in the editor's right-click menu.
