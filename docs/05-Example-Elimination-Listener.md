# 05 — Worked Example: The Elimination Listener

This document is a guided walkthrough of one feature that already exists in `B_ISW_AI`: the listener for `Lyra.Elimination.Message`. It is the cleanest example in the codebase of the *perception via gameplay message* pattern, and it shows how to keep the agent's belief base consistent when the world changes outside of its visual perception.

The goal: when a relevant pawn dies, the agent updates its blackboard so the behavior tree can react on the next tick.

In Kubík's framework, a gameplay message is a hybrid of **reaktivní komunikace** ([Kubík 2004, Chapter 4.2.1] — stigmergic traces in the environment) and **komunikační akt** ([Kubík 2004, Chapter 4.4] — explicit pub/sub messaging in a higher-level language). The game broadcasts a typed event with a well-defined semantics; agents that have subscribed treat it as a `vjem`. Crucially, this perception bypasses the visual `AIPerceptionComponent` — an agent on the other side of the map learns of an elimination even though it could not have seen it. This is the same trade-off Kubík discusses on p. 17 between stigmergy and direct communication: stigmergy is realistic but slow and lossy; direct messaging is omniscient but limited to events the system explicitly emits.

## What we want the listener to do

Two situations are relevant:

1. **Our agent itself dies.** Whether or not it was carrying the flag, we need to clear `CarryingFlag` and `WePickedUpFlag` so that — when this controller possesses the next pawn after respawn — the BT doesn't believe it's still carrying.
2. **The enemy who is currently carrying our flag dies.** We need to clear `OurFlagCaptured` and `FlagCarrier` so that the BT exits the "recover our flag" branch and falls back to whatever it was doing before.

These two checks are independent. A single elimination could be both (the bot itself is the flag carrier who died). The handler runs them sequentially without short-circuiting between them.

## Where the listener lives

In `B_ISW_AI`, on `BeginPlay → AsyncAction_ExperienceReady → OnReady`. Specifically: in the same phase that discovers the flag pads, after the pads have been set on the blackboard.

**Why not in `OnPossess`?** `OnPossess` runs on every respawn. If you registered the listener there, you would accumulate one duplicate listener per life — by the third death the handler would run three times per elimination, and so on.

The controller object survives across possessions, so registering once in `BeginPlay` is exactly correct.

## The handler, step by step

In Blueprint, the listener is a `Listen for Gameplay Messages` node configured with:

- **Channel:** `Lyra.Elimination.Message`
- **Payload Type:** `FLyraVerbMessage`

Its "On Message Received" exec wire goes into a small subgraph that runs the two checks below.

### Step 0 — Guard the blackboard

```
IsValid(BBComp) → Branch
  └── False: return  (defensive — should not happen post-OnPossess, but cheap)
  └── True:  continue
```

`BBComp` is the cached `Blackboard Component` we stored from `UseBlackboard`. See [03 — AI Architecture](03-AI-Architecture.md) for why this matters.

### Step 1 — Did our agent die?

We compare the eliminated player against **our own** PlayerState:

```
Self (AI Controller) → GetPlayerState        ← controller's PS, not pawn's
Payload.Target                                ← already a PlayerState
==
Branch:
  True:
    BBComp → SetValueAsBool("CarryingFlag",   false)
    BBComp → SetValueAsBool("WePickedUpFlag", false)
```

**Why the controller's `GetPlayerState` and not `GetControlledPawn → GetPlayerState`?** At the moment this message fires, the pawn may already be destroyed (it died, after all). The controller persists across the pawn's death and keeps a valid PlayerState reference. The pawn-routed path would dereference None.

**Why only these two keys?** When the next pawn spawns and we re-possess it in `OnPossess`, those are the per-life "I am carrying the enemy flag" beliefs that need to be false. Static beliefs (flag base locations) are untouched. Shared beliefs (`OurFlagCaptured`, `FlagCarrier`) are handled by Step 2.

### Step 2 — Did the enemy carrying *our* flag die?

We compare the eliminated player against the carrier we currently believe is holding our flag:

```
BBComp → GetValueAsObject("FlagCarrier") → As Pawn → GetPlayerState
Payload.Target
==
Branch:
  True:
    BBComp → SetValueAsObject("FlagCarrier",   None)
    BBComp → SetValueAsBool("OurFlagCaptured", false)
```

A few small things matter here:

- `GetValueAsObject` returns an `Object` reference. Cast it to `Pawn` (or `LyraCharacter`) before calling `GetPlayerState`. If the cast fails, fall through — the carrier was either never set or has already been cleaned up.
- We compare `PlayerState` to `PlayerState`. The payload's `Target` is a PS; the carrier we stored is a pawn whose PS we dereference. Both must be the same type for the comparison to be meaningful.
- When the carrier is cleared, the BT's `OurFlagCaptured == true` decorator goes false on the next tick. Because of the **Observer Aborts: Lower Priority** that we set on that decorator in [04 — Blackboard and Behavior Tree](04-Blackboard-and-BehaviorTree.md), the recover-flag subtree aborts immediately and the agent goes back to either stealing or carrying.

### Step 3 — Order, not flow control

Both checks always run. Even if Step 1 was true, we still test Step 2 — because the agent itself could be the flag carrier (it had previously stolen the enemy flag and was on the way home). If you short-circuit between them you will leave `FlagCarrier` pointing at a dead pawn, and any teammate's behavior tree referring to it will be confused.

## What the agent *cannot* learn from this message

Worth pausing on, because students sometimes overestimate what gameplay messages tell them:

- The message doesn't tell you whether the eliminated character was on your team. You have to derive that yourself (e.g. compare `FindTeamFromObject(Payload.Target)` to your own team).
- It doesn't tell you the *location* of the kill — you have to infer it from the pawn or read it before the pawn is destroyed.
- It is **server-side only**. Client-only code (like UI) cannot use it without going through a replicated property or RPC. AI controllers live on the server, so this is fine for us.

## Why this is the pattern worth copying

The elimination listener is small but exercises every important rule:

- Listener registered exactly once, in the lifecycle phase that survives respawn (`BeginPlay`, not `OnPossess`).
- Blackboard access guarded by `IsValid(BBComp)` and routed through the cached component.
- Identity check goes through `Controller → GetPlayerState`, never through the (possibly dead) pawn.
- Two independent state updates done in sequence, not nested in an `else`.
- The handler updates **beliefs**, not behavior. It changes blackboard values; it does not call `MoveTo`, `StopMovement`, or anything else. The BT decides what to do on its next tick based on the new beliefs.

The last point is the most important conceptually. In Kubík's formal model the message handler implements the `změna_stavu : P × I → I` function — it transforms the incoming percept into a new internal state. The action-selection function `akce : P × I → A` belongs to the behavior tree. Mixing the two (e.g. calling `MoveTo` from a message handler) collapses the separation that makes the agent debuggable: you lose the ability to ask "given that the agent currently believes X, what would it do?" because behavior is no longer a function of state alone.

Copy this shape for any other gameplay message you want to react to. The two flag messages (`Lyra.CaptureTheFlag.FlagPickedUp.Message` and `Lyra.CaptureTheFlag.FlagDelivered.Message`) follow the same template — they're already implemented in `B_ISW_AI` and you can read them as reinforcement of the pattern.
