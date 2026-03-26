# Zion Swarm Simulation Project

## Overview

This project is a prototype for a large-scale swarm simulation inspired by the **Zion invasion scene from *The Matrix Revolutions***. The goal is to create a visually dense, fluid, and overwhelming swarm of flying enemies that behave like a single cohesive entity rather than isolated units.

The design focuses on achieving the feeling of:

* Endless flow of enemies
* Fluid, organic motion (similar to ocean currents or snakes)
* A "fight the swarm, not the individual" gameplay experience
* A "survive a hopeless situation until help arrives" gameplay experience

---

## Core Concept

The swarm system is divided into **multiple layers of simulation**, each optimized for a different level of detail and gameplay interaction:

### 1. GPU Swarm (Planned)

* Simulated using **Compute Shaders**
* Handles the **majority of the swarm** (Intended up to 250.000 simple agents)
* Responsible for large-scale movement and visual density based on vector field
* Focused on performance and visual cohesion
* No individual collision
* Gameplay logic based on volumes and randomized hit based on density
* Entities are either dead or alive

### 2. C++ Backend Swarm (Current Focus)

* Implemented as a **native C++ DLL**
* Handles **mid-range drones**
* Provides:

  * Vector field-based movement
  * Curl noise turbulence
  * Target attraction influence (Players, Some NPCs, Static Targets)
  * Gameplay logic based on C++ simple sphere and line collisions with volumes
  * Entities are either dead or alive
  * Optimized for up to 10.000 of agents

### 3. Gameplay Units (Future)

* A small subset of drones transitions into:

  * Individual enemies
  * Controlled by standard Unity AI / physics
* Used when:

  * Close to some targets and not in a "swarm mode"
  * Engaged in "combat"
* Enables:

  * Individual Targeting 
  * Killing specific targets
  * Collision and damage systems (Possibly health or partial damage)

---

## Current State

### Implemented

* C++ DLL integration with Unity
* Drone simulation backend (CPU-based)
* Vector field sampling (FUNCTIONAL, BUT UNDER REFACTORING)
* Curl noise-based turbulence
* Up to 5000 drones simulated efficiently
* Debug visualization tools (Gizmos) (UNDER REFACTORING)

### Not Yet Implemented

* Compute Shader swarm system
* GPU/CPU hybrid transition
* Gameplay systems (organized spawning, targets, combat, damage, etc.)
* Spline-based flow fields
* No Go zones such as Level Limits and Fixed Obstacles
* Density-based swarm control

---

## Project Architecture

```
Zion/
│
├── Dll Project/        # C++ backend (simulation logic)
│
├── Unity Project/      # Unity game project
│   ├── Assets/
│   │   ├── Scripts/
│   │   ├── Plugins/    # DLL is placed here
│   │   └── ...
```

---

## C++ DLL Workflow

The C++ backend is compiled as a **native DLL** and used by Unity via `DllImport`.

### 1. Build the DLL

* Open the C++ project in Visual Studio
* Set configuration to:

  * `Release`
  * `x64`
* Build the project

Output:

```
SwarmSim.dll
```

---

### 2. Import into Unity

Copy the generated DLL into:

```
Unity Project/Assets/Plugins/
```

Unity will automatically detect and load the plugin.

---

### 3. Updating the DLL

When making changes:

1. Rebuild the DLL in Visual Studio
2. Replace the DLL inside `Assets/Plugins/`

⚠️ Notes:

* It is necesary to Close Unity Editor in order to replace the DLL as it is loaded whenever the editor is open.

---

### 4. Unity Integration

Functions are exposed using:

```csharp
[DllImport("SwarmSim")]
```

The C# side communicates with the C++ backend to:

* Generate Field
* Initialize drones
* Update simulation
* Sample movement directions
* Pass gameplay parameters

---

## Simulation Approach

### Vector Field Movement

Drones are guided by a **3D vector field** that defines their movement direction.

The field is influenced by:

* Base flow
* Curl noise (turbulence)
* Player attraction (planned)
* Environmental modifiers (planned)

---

### Curl Noise

The system uses **curl noise** to generate:

* Smooth swirling motion
* Vortex-like behavior
* Natural-looking flow patterns

This avoids:

* Random jitter
* Linear or artificial movement

---

### Performance Goals

* Handle **10.000 drones (CPU)**
* Scale to **250.000 (GPU planned)**
* Maintain real-time performance
* Minimize Unity-side overhead

---

## Design Philosophy

The system is designed around the idea that:

> The player should feel overwhelmed by a living, flowing entity — not a collection of individual enemies.

This leads to:

* Emphasis on **collective behavior**
* Reduced importance of individual units
* Dynamic density and flow instead of rigid formations

---

## Future Development

### Short Term

* Improve field generation and control (In Development)
* Add spline-driven flow fields
* Add player influence to swarm behavior
* Scale to Compute Shader

### Mid Term

* Implement **Compute Shader swarm system**
* Introduce **density-based swarm control**
* Add spline-driven "swarm attack"
* Improve visual cohesion of swarm

### Long Term

* Hybrid CPU/GPU simulation
* Seamless transition between:

  * GPU swarm → CPU drones → gameplay units

---

## Summary

This project explores a hybrid simulation approach to achieve:

* Massive swarm scale
* Real-time performance
* Fluid, organic motion
* Strong gameplay integration

While still in early stages, the foundation is being built to support a **large-scale, cinematic swarm system** inspired by one of the most iconic sci-fi battle scenes.

---
