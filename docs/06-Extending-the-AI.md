# 06 — Extending the AI

This document walks through the most common changes you will make. For each one it gives the safe procedure and lists the pitfalls students have hit in past iterations.

## Adding a new blackboard key

1. Open `Content/Bot/BB_ISW_Bot`.
2. Click **New Key** and choose the type. Bool, Vector, Object, and Float cover almost everything.
3. Name it after **what it means**, not where it comes from. `EnemyIsNearMyBase` is good; `PerceptionEvent42` is bad.
4. Add an entry to the table in [04 — Blackboard and Behavior Tree](04-Blackboard-and-BehaviorTree.md) so future students know what you added.

**Pitfalls:**

- If you read the new key from a BT decorator before anything has written to it, you get the type's default value (false / zero / None). That's usually fine — but if not, write it in `OnPossess`.
- If you intend to read the key from BT services / tasks via `GetOwnersBlackboard → GetValueAsX`, you pass the key **name as a string**. Mis-typing the name silently returns the default. Use a single named string constant where possible.

## Writing a new BT service

Services run while a subtree is active. Use them to maintain a derived belief (e.g. "is the carrier visible") or poll something cheap.

1. In the Content Browser, right-click → **AI → Behavior Tree Service**.
2. Open it. Override `Event Receive Tick` (or `Event Receive Tick AI` if you want the `Controlled Pawn` already wired).
3. From `GetOwnersBlackboard`, write to the key you want to update. **Use the key name as a string** — `SetValueAsBool(KeyName="MyKey", Value=...)`.
4. Drop the service onto a node in the BT. Tune the tick interval (a default of 0.5 s with random deviation is usually fine).

**Pitfalls:**

- `GetOwnersBlackboard` only works inside BT nodes. Don't try to use it from the controller.
- A service ticking 60 times per second is almost never necessary. Use intervals.
- If your service does a line trace, prefer Lyra's `LineOfSightTo` over a raw `LineTraceByChannel` — it already respects perception channel settings.
- Don't use a service to *do* things. Services update beliefs. Tasks do things.

## Writing a new BT task

A task is a concrete action: move somewhere, fire a weapon, equip a different item, drop the flag, etc. Tasks succeed, fail, or stay running (for "in-progress" tasks).

1. Content Browser → right-click → **AI → Behavior Tree Task**.
2. Override `Event Receive Execute AI`.
3. Do your work. At the end, call `Finish Execute` with `Success` or `Fail`. For a long-running task (movement, animation), don't call `Finish Execute` immediately — keep state on the task and call it from a callback (e.g. `OnMoveCompleted`).
4. Drop the task as a leaf in the BT. Configure its exposed parameters in the BT details panel.

**Pitfalls:**

- Forgetting to call `Finish Execute` leaves the BT stuck on your task forever. The gameplay debugger will show your task as the active node and nothing else will run.
- If you call `Finish Execute` synchronously on the same frame the task started, behaviors above it may not see the transition — this is rare but happens. The textbook fix is to delay one frame.
- Don't write *behavior-wide* state to instance variables on the task. Tasks can be re-entered. Use the blackboard for state that must persist.

## Adding a new gameplay-message listener

Use the [worked example](05-Example-Elimination-Listener.md) as a template. Summary of the rules:

- Register in `BeginPlay → AsyncAction_ExperienceReady → OnReady`, not in `OnPossess`.
- Inside the handler, identify "self" via the controller's own `GetPlayerState`, not the pawn's.
- Guard with `IsValid(BBComp)` before any blackboard access.
- Update beliefs, not behavior. Don't drive `MoveTo` from inside a message handler.

## Replacing the existing controller

If your assignment calls for a different agent architecture — a BDI-style deliberative agent [Kubík 2004, §2.9], an InteRRaP-style hybrid [Kubík 2004, §3.2.1], a utility-based selector, etc. — you have two options:

**Option A — Subclass `B_ISW_AI`.** Inherit, override `OnPossess`, leave the lifecycle and listeners alone. Use this if you want to reuse the perception and message wiring and only change the decision logic.

**Option B — Subclass `LyraPlayerBotController` directly.** Start blank. Use this if you want to remove the BT entirely and run your own decision loop on `Tick`.

For B, remember to:

- Manually replicate the lifecycle-phase split: ExperienceReady for static setup, `OnPossess` for per-life setup.
- Manually register gameplay-message listeners in the right phase.
- Set the `Default Pawn Class` in the experience to a Lyra character that has the equipment you need (weapons, flag-pickup component).

Either way, you need to point the experience asset (`B_ShooterGame_CaptureTheFlag` or your fork of it) at your new controller. Editing the experience is the only way to actually swap the controller — assigning it on the bot's Default Pawn won't work because Lyra spawns bots through the experience.

## Common pitfalls reference

These are all in the lessons memory, but worth restating in one place:

- **`GetBlackboard()` returns None before `RunBehaviorTree` runs.** Always cache the `UseBlackboard` output pin.
- **`FindTeamFromObject` returns -1 if the pawn isn't team-assigned yet.** Guard with `bIsPartOfTeam`. If false, `Delay 0.1` and retry.
- **`AsyncAction_ExperienceReady` fires once per experience load, not once per spawn.** Use it for static-data setup. Per-life setup goes in `OnPossess`.
- **The Lyra Shooting Service is shared across Lyra bots and fragile.** Never edit it. Gate it by parenting decisions instead.
- **Observer Aborts inside Simple Parallel background branches are unreliable.** Put aborting decorators on a Selector *above* the Simple Parallel.
- **`B_CaptureTheFlagScoring.GetFlagPadByTeam` has a race.** Iterate `GetAllActorsOfClass(B_GrantFlagPad)` directly.
- **`_FlagTeamIndex` inside `B_GrantFlagPad.GrantOrDeliverFlag` is a local var.** Use the class-level `PadTeamIndex` instead.
- **Listener registered in `OnPossess` duplicates per life.** Register once in `BeginPlay`.
- **`GetControlledPawn → GetPlayerState` inside an elimination handler may return None.** Use the controller's own `GetPlayerState`.

## A debugging routine that will save you hours

When your agent does the wrong thing, the question is almost always "what did it believe was true at that moment?". The fastest way to answer that:

1. Set a breakpoint on the BT node that fired (right-click → Add Breakpoint).
2. When PIE pauses, open the **Gameplay Debugger** and snapshot the blackboard.
3. Compare to what you *think* should have been true. Find the key that disagrees with reality.
4. Trace backwards: what was the last thing that wrote to that key? Was the writer ever called? Was it called with the right value?

This is the same loop you would run in any debugger — the only twist is that "state" lives on the blackboard rather than in local variables. Treat the blackboard inspector as your watch window.
