# Nacre Engine
Nacre Engine is a game engine that implements an Entity-Component-System (ECS) architecture. It is my personal project and is currently a work in progress.

Built with SFML (3.1.0).

*Build version is found in the latest commit.*

### Mission Statement

> The dedicated purpose of the Nacre Engine is to give me an easy way to make simple and small-scale games much more efficiently and in a structured manner. Additionally, it serves as a way for me to learn how the ECS structure works.
> 

---

## Table of Contents

- [ECS Architecture](#ecs-architecture)
- [Event System](#event-system)
- [Current Features](#current-features)
- [System Groups](#system-groups)
- [Built-in Components](#built-in-components) and [Built-in Events](#built-in-events)
- [How It Fits Together](#how-it-fits-together)
- [Tutorials](#tutorials)
- [Naming Conventions](#naming-conventions)
- [Short Names](#short-names)
- [Building](#building)
- [Project Structure](#project-structure)
- [Known Limitations](#known-limitations)

---

## ECS Architecture

- **Entity** - a unique ID (`std::uint32_t`) that represents an object. It holds no data or logic itself.
- **Component** - a plain data struct that describes one aspect of an entity's state (e.g. `Component::Position`, `Component::Velocity`).
- **System** - logic that loops over component data (e.g. `Update::move`).

## Event System

- **Dispatcher** - registers event types, links listeners to them, and calls them when an event fires.
- **Event** - a struct that carries the data for one occurrence (e.g. `Event::MouseClick`).
- **Listener** - a function that reacts to an event using the data it carries (e.g. `Listener::clickedButton`).

---

## Current Features

### Engine Core (`src/Engine/`)

- **Entity Manager** - hands out entity IDs from a fixed pool (`MAX_ENTITIES`, currently 1000), recycles IDs of destroyed entities, and tracks which entities are alive.
- **Component Manager** - keeps one component array per component type and looks it up by type. Component types must be registered before use. Every array is held behind a common interface so they can all be told to drop an entity's components when it is destroyed.
- **Component Array** - stores every instance of a single component type, keyed by entity (currently backed by an `std::unordered_map`). Supports insert, remove, lookup, existence checks, and iteration over all entries for systems.
- **Event Dispatcher** - an observer-pattern event system keyed by event type. Events are registered once, listeners are attached, and calling an event runs its listeners immediately in the order they were added. It lets systems trigger behavior directly instead of tagging entities with marker components (i.e. the "tag component" antipattern).
- **Nacre Coordinator** - a singleton (accessed through a helper inline function `nc()`) that wraps the entity manager, component manager, and event dispatcher behind one interface. It also deletes an entity together with all of its attached components, and can clear every entity at once (used on scene changes).

### Game Layer (`src/Game/`)

- **Systems** - free functions that manipulate component arrays, keeping logic separate from data. They are organized into four groups by role: `Start`, `Control`, `Update`, and `Render` (see [System Groups](#system-groups)).
- **Scenes** - self-contained game states (currently Menu and Playing). Each scene creates its own entities, runs its own game loop, and is switched through the `PlayScene` event, which clears all entities before the next scene starts.
- **Entity Maker** - helper functions (e.g. `Maker::player`) that create an entity and attach the right set of components from a list of parameters
- **Sprites and Textures** - 2D sprites built from a `Component::Texture`. Textures are loaded once per scene into a container keyed by `Enum::Texture`, and each sprite is automatically scaled to its entity's `Component::Transform` size and tinted by its `Component::Color`.
- **Text** - text entities with a string, size, color, and one of 9 alignment formats (center, corners, edges).
- **Origin Points** - choose which point of an entity (center, a corner, or an edge midpoint) its position refers to.
- **Buttons** - clickable entities with idle, hovered, and clicked states. Each state plays a timed elastic scale animation, and a button can optionally send the player to another scene once its click animation finishes.
- **Z-Index Rendering and Layers** - entities are sorted by their `ZIndex` every frame and drawn lowest to highest, with entities at the same index drawn in entity ID order. Setting `visible` to false skips drawing. Makers take a named `Enum::Layer` (`MAIN` for gameplay entities such as the player, `UI` for interface elements such as buttons, `TEXT` for text such as titles) instead of a raw index number.
- **Movement** - velocity with min/max limits, acceleration, drag, and bounce-off-boundaries, all scaled by delta time. Includes WASD player control.
- **Mouse Input** - mouse positions are converted into world coordinates and dispatched as `Event::MouseClick` and `Event::MouseMoved`. Button hit-testing uses an axis-aligned bounding box (AABB) check.
- **Configuration** - window size, FPS cap, and title live in `Config.hpp`, along with the `Asset::window()` and `Asset::font()` accessors inline functions.
- **Custom App Icon** - embeds `resources/nacre.ico` as the executable's icon through `app.rc` (Windows only)

The coordinate system is centered on the window, so `(0, 0)` is the middle of the screen. As in SFML, x increases rightward and y increases downward.

## System Groups

Every system belongs to one of four groups, and each group is a namespace. A group describes the role a system plays, not any specific feature.

| Group | Represents | When it runs |
| --- | --- | --- |
| `Start` | **Setup** - prepares entities and resources so they are ready to use | Once, when a scene begins, after its entities are created |
| `Control` | **Input** - translates player input into changes in component data | Every frame, before `Update` |
| `Update` | **Logic** - advances the game state over time | Every frame |
| `Render` | **Output** - prepares the current game state into what appears on screen | Every frame, after `Update` |

---

## Built-in Components

| Component | Purpose |
| --- | --- |
| `Position` | World position (`x`, `y`) |
| `Transform` | Size (`width`, `height`) |
| `OriginPoint` | Which anchor point the position refers to |
| `OriginOffsets` | The calculated offsets for that anchor point |
| `Sprite` | Holds the SFML sprite |
| `Texture` | Which texture the sprite uses |
| `Color` | Tint applied to the sprite |
| `Text` | SFML text plus its string, size, color, and alignment |
| `ZIndex` | Draw order and visibility |
| `Button` | Enabled flag, current state, animation durations, and timer |
| `NextScene` | The scene to play after a button is clicked |
| `Velocity` | Current velocity with min/max limits |
| `Speed` | Acceleration applied from input |
| `Drag` | How quickly velocity slows toward zero |
| `PlayerController` | Enables keyboard control for an entity |

## Built-in Events

| Event | Purpose |
| --- | --- |
| `SetSpriteOrigin` | Applies an origin point to a sprite |
| `SetColor` | Applies a color tint to a sprite |
| `LoadSprite` | Builds a sprite from a loaded texture and scales it to the entity's size |
| `SetTextString` / `SetTextSize` / `SetTextColor` / `SetTextOrigin` | Apply text properties |
| `MouseClick` | Mouse button pressed, with world position |
| `MouseMoved` | Mouse moved, with world position |
| `PlayScene` | Clears all entities and starts another scene |

---

## Project Structure

```
docs/
└── TUTORIALS.md    step-by-step guides
src/
├── Engine/    reusable, header-only ECS core
└── Game/      game code built on the engine
    ├── Headers/    components, events, enums, config, and declarations
    ├── Scenes/     the scene loops and scene switching
    ├── EntityMaker.cpp
    ├── Listeners.cpp
    ├── Systems.cpp
    └── Game.cpp    registration, listener linking, and program entry point
```

---

## How It Fits Together

1. At startup (`Game.cpp`), components and events are registered, and listeners are linked to events.
2. A scene creates its entities, which are built from components.
3. The scene runs its `Start` systems once.
4. The scene loop then runs its `Control`, `Update`, and `Render` systems every frame. Systems read and modify component data, and fire events when something else needs to react.

The [tutorials](docs/TUTORIALS.md) walk through each of these pieces using code that already exists in the project.

---

## Tutorials

Step-by-step guides that use code already in the project are in [docs/TUTORIALS.md](docs/TUTORIALS.md):

- [Implementing a Scene](docs/TUTORIALS.md#implementing-a-scene)
- [Instantiating an Entity](docs/TUTORIALS.md#instantiating-an-entity)
- [Creating a Component](docs/TUTORIALS.md#creating-a-component)
- [Implementing a System](docs/TUTORIALS.md#implementing-a-system)
- [Implementing an Event and Listener](docs/TUTORIALS.md#implementing-an-event-and-listener)
- [Adding a Texture](docs/TUTORIALS.md#adding-a-texture)
- [Adding a Layer](docs/TUTORIALS.md#adding-a-layer)

---

## Naming Conventions

| Item | Convention | Examples |
| --- | --- | --- |
| Files | `PascalCase`. `.hpp` for headers, `.cpp` for source. | `EntityMaker.cpp`, `NacreCoordinator.hpp` |
| Header guards | `FILE_NAME_HPP` | `ENTITY_MAKER_HPP` |
| Classes, structs, enums | `PascalCase` | `EntityManager`, `Component::Position`, `Enum::TextFormat` |
| Namespaces | `PascalCase`, named for the role of what they group | `Component`, `Event`, `Listener`, `Maker`, `Update` |
| Functions and methods | `camelCase`, starting with a verb | `createEntity`, `setSpriteOrigin`, `applyButtonHoverAnimation` |
| Variables and parameters | `camelCase` | `renderQueue`, `loadedTextures`, `hoverAnimationDuration` |
| Constants | `SCREAMING_SNAKE_CASE` | `MAX_ENTITIES`, `BOUNCE_FACTOR` |
| Enum values | `SCREAMING_SNAKE_CASE` | `Enum::OriginPoint::TOP_LEFT`, `Scene::MENU` |
| Template type parameter | `T` | `ComponentArray<T>` |

### Engine-specific conventions

- **Components** are singular nouns for the data they hold (`Position`, `Velocity`, `Drag`). A component that wraps a single value names it `data` (`Color`, `Texture`, `OriginPoint`).
- **Component array variables** are named `<component>Array`: `positionArray`, `buttonArray`, `spriteArray`.
- **Systems** live in the namespace of their [group](#system-groups), start with a verb, and name what they act on: `Start::loadSprites`, `Update::move`, `Render::doZIndex`.
- **Events** are named for what they ask for or what happened. Requests start with a verb (`SetColor`, `LoadSprite`, `PlayScene`), and reports describe the occurrence (`MouseClick`, `MouseMoved`). An event aimed at one entity has the `target` entity as its first member.
- **Listeners** are `camelCase` functions in `Listener`. A listener that carries out a request mirrors the event's name (`Event::SetColor` is handled by `Listener::setColor`). A listener that reacts to a report is named for what it does (`Event::MouseClick` is handled by `Listener::clickedButton`).
- **Entity Makers** are lowercase nouns for the entity they create: `Maker::player`, `Maker::button`, `Maker::text`.
- **Scenes** use an enum value (`Scene::PLAYING`), a function named `<name>Scene` (`playingScene`), and a file named `<Name>Scene.cpp` (`PlayingScene.cpp`).
- **Singletons** are accessed with `getInstance()`. The coordinator also has the shorthand `nc()`.
- **Parameters that are not modified are `const`**, as in `const Entity player` and `const DeltaTime dt`.

### Formatting

The codebase uses tab indentation and puts braces on their own line. The examples below use generic names to show the shape of each construct.

**Functions and control flow.** Every body, including a single `continue`, gets its own braces:

```cpp
void doSomething(const DeltaTime dt)
{
	for (auto& [entity, thing] : thingArray->getAll())
	{
		if (!otherArray->hasData(entity))
		{
			continue;
		}

		thing.value += dt;
	}
}
```

A short guard that throws may stay on one line:

```cpp
if (event.target == NULL_ENTITY) throw std::runtime_error("Error: null entity");
```

**Namespaces, structs, and enums.** Contents are indented one level inside the braces:

```cpp
namespace Group
{
	void doSomething(const DeltaTime dt);
}

struct Thing
{
	double x = 0.0;
	double y = 0.0;
};

enum class Kind
{
	FIRST,
	SECOND,
};
```

**Long parameter lists.** The opening parenthesis goes on its own line, followed by one parameter per line:

```cpp
Entity makeThing
(
	const int size,
	const sf::Color col,
	const sf::Vector2f pos
);

Entity makeSomethingElse();
```

**Calls with several arguments.** These are split the same way. Short calls with one argument stay on a single line:

```cpp
Group::doSomething
(
	firstArgument,
	secondArgument
);

Group::doSomethingElse(dt);
```

**Braced values.** The opening brace goes on its own line, one value per line, including when it is nested inside a call:

```cpp
nc().addComponent
(
	entity,
	Component::Thing
	{
		valueA,
		valueB
	}
);

sf::Vector2f
{
	0.f,
	-200.f
}
```

**Comments.** Comments are short and lowercase, and describe the block that follows:

```cpp
// game state variables
sf::Clock clock;
```

## Short Names

These abbreviations are used throughout the code. Anything not listed here is spelled out in full.

| Short | Stands for | Where it's used |
| --- | --- | --- |
| `pos` | position (`Component::Position`) | local variables and `Maker` parameters |
| `trans` | transform (`Component::Transform`) | local variables and `checkCollision` parameters |
| `dt` | delta time (`DeltaTime`), the seconds since the last frame | systems and scene loops |
| `col` | color | `Maker` parameters |
| `str` | string | `Maker` parameters for text content |
| `nc` | `NacreCoordinator` | the `nc()` accessor |
| `cm`, `em`, `ds` | `ComponentManager`, `EntityManager`, `Dispatcher` | the coordinator's internal references |
| `i` | loop index | `for` loops |
| `it` | iterator | lookups in the managers and dispatcher |
| `a`, `b` | the two things being compared | `checkCollision` |
| `T` | a component or event type | templates |

---

## Building

- Currently only supports building for Windows and MSVC.
- Requires CMake 3.28+ and a C++17 compiler.
- SFML 3.1.0 is downloaded and statically linked automatically.

```
cmake -B build
cmake --build build
```

The executable (`NacreEngine`) is output under `build/bin/`.

The engine loads its assets from a `resources/` folder (the font `arial.ttf`, a `sprites/` folder, and `nacre.ico`). In a normal build this points at the `resources/` folder in the project root. Configure with `-DPRODUCTION_BUILD=ON` to load from a `resources/` folder next to the executable instead, and to hide the console window on MSVC (requires a clean `build/` directory).

## Known Limitations

- The entity count is fixed at `MAX_ENTITIES` (1000).
- Component storage uses hash maps, not densely packed arrays.
- Clearing all entities loops over every possible entity ID.
- Switching scenes calls the next scene's function from inside the current scene's loop, so scenes nest on the call stack.
- The build is currently set up for Windows and MSVC.
- SFML's audio and network modules are not linked yet (for faster build times).