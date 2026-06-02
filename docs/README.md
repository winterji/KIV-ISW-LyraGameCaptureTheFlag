# ShooterCTF — Student Documentation

> Czech version: [docs/cs/](cs/README.md) — *Česká verze této dokumentace.*

Welcome. This documentation is for students of the **KIV/ISW** course who will implement AI agents inside the `ShooterCTF` game mode as a semestral project. The documentation draws from the book **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno, 2004) and the terminology used in this documentation comes from this book.

The game mode is a Capture the Flag arena built on top of [Lyra Starter Game](https://www.unrealengine.com/marketplace/en-US/product/lyra) in Unreal Engine 5.7. It ships with a baseline Blueprint AI (`B_ISW_AI`) that you will **inherit from** to build your own agent. The point is to give you a real game environment in which an agent must perceive the world, decide, and act — without you having to build the engine plumbing yourself.

In Kubík's terminology, the baseline `B_ISW_AI` is a **BDI-inspired reactive agent**: it has an explicit belief base (the Blackboard), implicit goals encoded in BT decorator preconditions, and implicit intentions as the currently-active subtree path. It does not implement the canonical BDI architecture from Kubík in full form (no plan library, no explicit `options() → filter()` deliberation cycle, no first-class desire/intention data) — for the full discussion see [03 — AI Architecture, §3.3.2](03-AI-Architecture.md#332-is-the-baseline-bdi). Your work will typically push it toward a more deliberative or hybrid architecture.

## How to read this documentation

If this is your first time touching the project, read them in order:

| #   | Document                                                          | What it covers                                                                                                  |
| --- | ----------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------- |
| 01  | [Setup](01-Setup.md)                                              | Install Lyra, enable the plugin, launch the CTF experience.                                                     |
| 02  | [Game Mode](02-Game-Mode.md)                                      | CTF rules, the actors involved, gameplay flow, scoring.                                                         |
| 03  | [AI Architecture](03-AI-Architecture.md)                          | Mapping Kubík's agent concepts onto the Lyra AI stack. Controller lifecycle. What "perception → decision → action" looks like in Blueprint. |
| 04  | [Blackboard & Behavior Tree](04-Blackboard-and-BehaviorTree.md)   | Every blackboard key, what writes it, what reads it. Walkthrough of the existing `BT_ISW_CTF_bot`.              |
| 05  | [Example — flag pickup listener](05-Example-Elimination-Listener.md) | A complete worked example of a gameplay-message listener that updates the agent's beliefs.                  |
| 06  | [Extending the AI](06-Extending-the-AI.md)                        | How to create your own agent by inheriting from `B_ISW_AI`. Adding BB keys, BT services, BT tasks, message listeners, and cooperative behavior. Common pitfalls. |
| 07  | [Assignments](07-Assignments.md)                                  | Suggested project ideas at varying difficulty levels, each anchored to a specific chapter of Kubík.             |

## What you will need

- **Unreal Engine 5.7** (the version Lyra was built against).
- **Lyra Starter Game** with this plugin under `Plugins/GameFeatures/ShooterCTF/`.
- Basic comfort with **Blueprint visual scripting**. No C++ knowledge is required.
- The book **Kubík, A. — *Inteligentní agenty*** (Computer Press, Brno 2004, ISBN 80-251-0323-4). The whole conceptual vocabulary used throughout these docs (vjem / akce / vnitřní stav, reaktivní / deliberativní / sociální / hybridní agent, subsumpční architektura, BDI, stigmergie, kontraktační síť, …) comes from this book.

## What's already built

- A working CTF game mode with flag pickup, delivery, and capture scoring.
- A baseline Blueprint AI (`B_ISW_AI`) that:
  - learns where its own and the enemy flag bases are on spawn;
  - chases and shoots an enemy flag carrier with line-of-sight gating;
  - reacts to gameplay messages about flag pickup / delivery and character elimination;
  - when it dies, the flag returns to base if it was carrying it (see [05](05-Example-Elimination-Listener.md)).

You will build your own agent by **inheriting from `B_ISW_AI`** — your child Blueprint keeps all of this wiring and adds your own Behavior Tree and any additional listeners on top. The baseline is intentionally **not** a strong opponent; you are meant to do better.

## What we won't do for you

Your task will be to **design and implement at least one non-trivial AI behavior** on top of this baseline. The documentation tells you what state the AI controller is in now and how to approach extending it. I recommend actually reading at least [AI Architecture](03-AI-Architecture.md) and [Extending the AI](06-Extending-the-AI.md) where important concepts and procedures are explained along with common pitfalls. Assignments with tips and recommendations can be found in [Assignments](07-Assignments.md).

## Further available materials

| Description | URL |
| ----------- | ---- |
| Lyra Sample Game documentation | https://dev.epicgames.com/documentation/unreal-engine/lyra-sample-game-in-unreal-engine |
| YT — overview of the default AI controller in Lyra | https://www.youtube.com/watch?v=jZFgTEGRJxg |
| UE5 docs — AI controller | https://dev.epicgames.com/documentation/unreal-engine/behavior-tree-in-unreal-engine---quick-start-guide |

## Conventions used in this documentation

- File and asset paths are written relative to the plugin root, e.g. `Content/Bot/B_ISW_AI`.
- "BB" = Blackboard. "BT" = Behavior Tree. "BTS" = BT Service. "BTT" = BT Task.
- Blueprint nodes are shown with the same name they have in the editor's right-click menu (in English, because UE5 does not translate node names).
- Kubík citations are written as `[Kubík 2004, Chapter X.Y]`, where `Chapter X.Y` is the chapter number in the book.
