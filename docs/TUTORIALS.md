# Nacre Engine Tutorials

Step-by-step guides for building with Nacre Engine. Every example uses code that already exists in the project. For an overview of the engine itself, see the [README](../README.md).

## Contents

- [Implementing a Scene](#implementing-a-scene)
- [Instantiating an Entity](#instantiating-an-entity)
- [Creating a Component](#creating-a-component)
- [Implementing a System](#implementing-a-system)
- [Implementing an Event and Listener](#implementing-an-event-and-listener)
- [Adding a Texture](#adding-a-texture)
- [Adding a Layer](#adding-a-layer)

For the project's naming style and abbreviations, see [Naming Conventions](../README.md#naming-conventions) and [Short Names](../README.md#short-names).

---

## Implementing a Scene

A scene is a plain function that owns a set of entities and its own game loop. The Menu scene (`MenuScene.cpp`) and the Playing scene (`PlayingScene.cpp`) are the examples used here.

### 1. Add the scene to the enum and declare its function

In `src/Game/Headers/Scenes.hpp`, add a value to `Scene` and declare the scene function. `PLAYING` and `playingScene()` were added this way:

```cpp
enum class Scene
{
    MENU,
    PLAYING,
};

void playScene(Scene scene);
void menuScene();
void playingScene();
```

### 2. Add a case to `playScene()`

In `src/Game/Scenes/SceneManager.cpp`, route the new enum value to the new function:

```cpp
switch (scene) {
case Scene::MENU:
    menuScene();
    break;
case Scene::PLAYING:
    playingScene();
    break;
default:
    throw std::runtime_error("Scene does not exist.");
    break;
}
```

### 3. Create the scene file

Create `src/Game/Scenes/<Name>Scene.cpp`. CMake collects every `.cpp` under `src/` automatically, so nothing needs to be added to `CMakeLists.txt` by hand.

### 4. Write the scene function

A scene has four parts: state variables, entity instantiation, `Start` systems, and the loop. This is `playingScene()`:

```cpp
#include <SFML/Graphics.hpp>

#include <optional>
#include <queue>

#include "../src/Game/Headers/GameManager.hpp"
#include "../src/Game/Headers/Scenes.hpp"

void playingScene()
{
    // game state variables
    sf::Clock clock;
    std::queue<Entity> renderQueue;

    Component::Singleton::TexturesContainer loadedTextures{};

    // entity instantiation (see "Instantiating an Entity")
    Entity player = Maker::player(/* ... */);

    // onstart systems
    Start::loadTextures(loadedTextures);
    Start::loadSprites(loadedTextures);
    Start::setSpriteOrigin();
    Start::setColor();

    while (Asset::window().isOpen())
    {
        DeltaTime dt = clock.restart().asSeconds();

        while (const std::optional event = Asset::window().pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                Asset::window().close();
            }
        }

        // systems
        Control::doPlayerControl(player, dt);
        Update::boundaryBounce
        (
            -(Config::SCREEN_WIDTH / 2.0),
            (Config::SCREEN_WIDTH / 2.0),
            -(Config::SCREEN_HEIGHT / 2.0),
            (Config::SCREEN_HEIGHT / 2.0)
        );
        Update::move(dt);
        Update::drag(dt);

        Asset::window().clear();
        // render systems
        Render::doZIndex(renderQueue);
        Render::render(Asset::window(), renderQueue);
        Asset::window().display();
    }
}
```

A few things to keep in mind:

- **Order of `Start` systems matters.** Sprites must exist before they can be given an origin or a color, so `loadSprites` runs before `setSpriteOrigin` and `setColor`. Text origin depends on the text's string and size, so `setText` runs before `setTextOrigin` (as in `menuScene()`).
- **Systems run in a deliberate order each frame.** Here input (`Control`) comes first, then logic (`Update`), then drawing (`Render`).
- **Include `GameManager.hpp`.** It pulls in the coordinator, components, systems, maker, events, listeners, and config in one include.

### 5. If the scene has buttons, forward mouse events

Buttons only react to the `MouseClick` and `MouseMoved` events, so a scene with buttons must dispatch them from its loop, as `menuScene()` does. In its event loop (`pixelPos` and `worldPos` are the mouse position in window and world coordinates):

```cpp
const sf::Vector2i pixelPos = sf::Mouse::getPosition(Asset::window());
const sf::Vector2f worldPos = Asset::window().mapPixelToCoords(pixelPos);

// ... inside the pollEvent loop:
if (const auto& mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
{
    if (mousePressed->button == sf::Mouse::Button::Left)
    {
        nc().call<Event::MouseClick>(Event::MouseClick{ worldPos });
    }
}
else if (event->is<sf::Event::MouseMovedRaw>() ||
    event->is<sf::Event::MouseLeft>() ||
    event->is<sf::Event::MouseEntered>())
{
    nc().call<Event::MouseMoved>(Event::MouseMoved{ worldPos });
}
```

The scene's update loop also needs the button systems, in this order (from `menuScene()`):

```cpp
Update::doButtonTimer(dt);
Update::applyButtonHoverAnimation();
Update::applyButtonIdleAnimation();
Update::applyButtonClickedAnimation();
Update::enableButtonNextScene();
```

### 6. Switch to the scene

- **From a button:** pass the scene as the `scene` argument of `Maker::button`. The Menu's play button uses `Scene::PLAYING`. When the button's click animation finishes, `Update::enableButtonNextScene()` fires the `PlayScene` event, and `Listener::playNextScene` clears every entity and plays the next scene:

  ```cpp
  void playNextScene(Event::PlayScene event)
  {
      nc().destroyAll();
      playScene(event.next);
  }
  ```

- **As the first scene:** `Game.cpp` starts the game with `playScene(Scene::MENU);`. Change the argument to start somewhere else.

---

## Instantiating an Entity

Entities are created through the coordinator. The Entity Maker functions (`Maker::player`, `Maker::button`, `Maker::text`) wrap that up so an entity of a common type is created with every component it needs in a single call.

### Using an existing Maker

This creates the title text in `menuScene()`. The parameters are, in order: text size, font, color, layer, string, position, and alignment format.

```cpp
Entity title = Maker::text
(
    64,
    Asset::font(Asset::Font::MAIN),
    sf::Color::White,
    Enum::Layer::TEXT,
    "Nacre Engine",
    sf::Vector2f
    {
        0.f,
        -200.f
    },
    Enum::TextFormat::CENTER
);
```

This creates the play button in the same scene. Its animation durations are constants declared at the top of `MenuScene.cpp` (`DEFAULT_HOVER_ANIMATION_DURATION = 0.4`, `DEFAULT_IDLE_ANIMATION_DURATION = 0.4`, `DEFAULT_CLICKED_ANIMATION_DURATION = 0.15`):

```cpp
Entity playButton = Maker::button
(
    64,                             // text size
    Scene::PLAYING,                 // scene to play when clicked
    DEFAULT_HOVER_ANIMATION_DURATION,
    DEFAULT_IDLE_ANIMATION_DURATION,
    DEFAULT_CLICKED_ANIMATION_DURATION,
    sf::Color::Red,                 // button color
    sf::Color::White,               // text color
    Asset::font(Asset::Font::MAIN),
    Enum::Layer::UI,
    "Play",                         // button text
    sf::Vector2f{ 0.f, 0.f },       // position
    sf::Vector2f{ 200.f, 100.f },   // size
    Enum::TextFormat::CENTER,
    Enum::OriginPoint::CENTER,
    Enum::Texture::TEXTURE_PLACEHOLDER
);
```

After creating entities, run the scene's `Start` systems so their SFML objects are set up (see [Implementing a Scene](#implementing-a-scene)). `Maker::text`, for example, only stores the text's settings. `Start::setText()` and `Start::setTextOrigin()` are what apply them.

### Writing a new Maker

1. Declare it in `src/Game/Headers/EntityMaker.hpp`, inside `namespace Maker`.
2. Define it in `src/Game/EntityMaker.cpp`: create the entity, add each component, and return the entity. This is `Maker::text` (condensed):

```cpp
Entity text
(
    const int size,
    const sf::Font& font,
    const sf::Color col,
    const Enum::Layer layer,
    const std::string str,
    const sf::Vector2f pos,
    const Enum::TextFormat format
)
{
    Entity entity = nc().createEntity();

    nc().addComponent(entity, Component::Position{ pos.x, pos.y });
    nc().addComponent(entity, Component::Text{ sf::Text(font), str, size, col, format });
    nc().addComponent(entity, Component::ZIndex{ static_cast<int>(layer), true });
    nc().addComponent(entity, Component::Transform{}); // unused but required

    return entity;
}
```

### Things to know

- **Add every component the entity's systems need.** Systems skip entities that are missing a component they expect, so a missing component usually means the entity silently does nothing. Reading a missing component directly with `getComponent` or `getData` throws, so check with `hasData` first.
- **Components must be registered first.** Every component type is registered in `Game.cpp` before any scene runs (see [Creating a Component](#creating-a-component)).
- **Pick a layer.** Every Maker takes an `Enum::Layer` that sets the entity's draw order, instead of a raw number. The title in `menuScene()` uses `TEXT`, the play button uses `UI`, and the player in `playingScene()` uses `MAIN`. See [Adding a Layer](#adding-a-layer).
- **Entities can be created without a Maker.** A Maker is just `nc().createEntity()` followed by `nc().addComponent(...)` calls.
- **Limit:** at most `MAX_ENTITIES` (1000) entities can be alive at once. Creating more throws.

### Destroying entities

- `nc().deleteEntity(entity)` removes the entity's components and frees its ID for reuse.
- `nc().destroyAll()` does the same for every entity. It is what a scene change uses.
- `nc().isAlive(entity)` checks whether an ID is currently in use.

---

## Creating a Component

A component is a plain struct that holds data and nothing else. `Component::Drag` is the example used here.

### 1. Define the struct

In `src/Game/Headers/Components.hpp`, inside `namespace Component`:

```cpp
struct Drag
{
    double x = 0.0;
    double y = 0.0;
};
```

### 2. Register it

In `Game.cpp`, with the other component registrations, before any scene runs:

```cpp
nc().registerComponent<Component::Drag>();
```

Registering a component twice throws `"Component already registered."`, and using a component that was never registered throws `"Component not registered."`.

### 3. Attach it to an entity

Usually this happens inside a Maker. `Maker::player` attaches `Drag`:

```cpp
nc().addComponent
(
    entity,
    Component::Drag
    {
        drag.x,
        drag.y
    }
);
```

### 4. Use it in a system

`Update::drag` loops over every entity that has a `Velocity` and applies its `Drag` (see [Implementing a System](#implementing-a-system)). To read or change a single entity's component outside a system, use `nc().getComponent<Component::Drag>(entity)`.

### Patterns used by existing components

- **Plain data with defaults.** Give every member a default value, as in `Drag` and `Position`. Components hold no logic.
- **Single-value wrappers use a `data` member.** `Color`, `Texture`, and `OriginPoint` each wrap one value as `data`.
- **SFML types without a default constructor are wrapped in `std::optional`.** `Sprite::body` and `Text::box` are filled in later by the `Start` systems.
- **Data shared by a whole scene isn't a component.** It goes in `Component::Singleton` (for example `TexturesContainer`) and is passed to the systems that need it. It is not registered.

---

## Implementing a System

A system is a function that loops over the entities that have a particular component and reads or changes their data. `Update::move` is the example used here.

### 1. Pick a group

Choose the [System Group](../README.md#system-groups) that matches the system's role. Moving entities over time is logic, so `move` belongs to `Update`.

### 2. Declare it

In `src/Game/Headers/Systems.hpp`, inside the group's namespace:

```cpp
namespace Update
{
    void move(const DeltaTime dt);
}
```

### 3. Define it

In `src/Game/Systems.cpp`:

```cpp
void Update::move(const DeltaTime dt)
{
    auto& velocityArray = nc().getComponentArray<Component::Velocity>();
    auto& positionArray = nc().getComponentArray<Component::Position>();

    for (auto& [entity, velocity] : velocityArray->getAll())
    {
        if (!positionArray->hasData(entity))
        {
            continue;
        }

        auto& pos = positionArray->getData(entity);
        pos.x += (velocity.x * dt);
        pos.y += (velocity.y * dt);
    }
}
```

Most systems follow the same shape:

1. Get the component arrays the system needs from the coordinator.
2. Loop over `getAll()` of the component that drives the system, using the entity as the key.
3. Skip entities that are missing any other component the system needs, using `hasData`.
4. Read or modify the data.

### 4. Call it from a scene loop

```cpp
Update::move(dt);
```

### Things to know

- **Time-based systems take `const DeltaTime dt`** so movement is independent of frame rate.
- **Pass scene-specific data as parameters.** `Control::doPlayerControl(player, dt)` takes the player entity, and `Update::boundaryBounce(left, right, top, bottom)` takes the boundaries.
- **Order matters.** In `playingScene()`, `Control` runs first, then `boundaryBounce`, `move`, and `drag`.
- **A system can hand work to a listener.** When an SFML object has to be configured, the system loops over entities and fires an event for each one instead of touching the object itself. `Start::setColor` does this (see the next tutorial).

---

## Implementing an Event and Listener

An event is a struct carrying data. A listener is a function that reacts to that event. `Event::SetColor` and `Listener::setColor` are the example used here.

### 1. Define the event

In `src/Game/Headers/Events.hpp`, inside `namespace Event`:

```cpp
struct SetColor
{
    Entity target = NULL_ENTITY;
    const Component::Color& color;
};
```

By convention the first member is the `target` entity (defaulting to `NULL_ENTITY`), followed by references to the data the listener needs. Events that aren't about one entity simply leave `target` out, like `MouseClick`, which carries only a position.

### 2. Write the listener

Declare it in `src/Game/Headers/Listeners.hpp` inside `namespace Listener`:

```cpp
void setColor(Event::SetColor event);
```

Define it in `src/Game/Listeners.cpp`:

```cpp
void setColor(Event::SetColor event)
{
    if (event.target == NULL_ENTITY) throw std::runtime_error("Error: null entity");

    auto& spriteArray = nc().getComponentArray<Component::Sprite>();

    if (!spriteArray->hasData(event.target))
    {
        return;
    }

    auto& sprite = spriteArray->getData(event.target);

    sprite.body->setColor(event.color.data);
}
```

### 3. Register the event and link the listener

In `Game.cpp`. The event must be registered before a listener is linked to it:

```cpp
nc().registerEvent<Event::SetColor>();

nc().listen<Event::SetColor>(Listener::setColor);
```

### 4. Fire the event

Use `nc().call<...>()` from a system or a scene. `Start::setColor` fires `SetColor` once for every entity that has a `Color`:

```cpp
void Start::setColor()
{
    auto& colorArray = nc().getComponentArray<Component::Color>();

    for (auto& [entity, color] : colorArray->getAll())
    {
        nc().call<Event::SetColor>
        (
            Event::SetColor
            { entity, color }
        );
    }
}
```

### Things to know

- **Listeners run immediately**, in the order they were linked, before `call` returns.
- **One event can have several listeners.** `MouseMoved` is linked to both `Listener::hoverButton` and `Listener::idleButton`.
- **Events carry references to component data**, so a listener can modify it. `SetSpriteOrigin`, for example, writes the calculated offsets back into the entity's `OriginOffsets`.
- **Calling or listening to an unregistered event throws** `"Event not registered."`.
- **Use an event when something needs to react to something else happening**, such as a click, a scene change, or an SFML object needing setup, instead of tagging entities with a marker component for a system to look for.

---

## Adding a Texture

Textures are identified by an enum value and loaded once per scene. `TEXTURE_PLACEHOLDER` is the example used here.

1. **Add the image** to `resources/sprites/`. The existing texture is `placeholder_texture.jpeg`.
2. **Add an enum value** to `Enum::Texture` in `src/Game/Headers/Enums.hpp`:

   ```cpp
   enum class Texture
   {
       TEXTURE_PLACEHOLDER,
   };
   ```

3. **Load it** in `Start::loadTextures` in `Systems.cpp`. `SPRITES_PATH` is defined by CMake and points to `resources/sprites/`:

   ```cpp
   loadedTextures.map.emplace(Enum::Texture::TEXTURE_PLACEHOLDER, sf::Texture(SPRITES_PATH "placeholder_texture.jpeg"));
   ```

4. **Use it** by passing the enum value as the `texture` argument of a Maker (`Maker::player` and `Maker::button` both take one), for example `Enum::Texture::TEXTURE_PLACEHOLDER`. `Start::loadSprites` then builds each sprite from it.

If an entity uses a texture that was never loaded, `Listener::loadSprite` throws `"Texture has not been loaded."`.

---

## Adding a Layer

Layers give each entity a named draw order. A layer's number is the entity's z-index, and entities are drawn from the lowest number to the highest. `MAIN`, `UI`, and `TEXT` are the existing layers.

1. **Add a value** to `Enum::Layer` in `src/Game/Headers/Enums.hpp`. The number is the draw order:

   ```cpp
   enum class Layer
   {
       MAIN = 0,
       UI = 1,
       TEXT = 0
   };
   ```

2. **Pass it to a Maker** as its `layer` argument. The title uses `Enum::Layer::TEXT`, the play button uses `Enum::Layer::UI`, and the player uses `Enum::Layer::MAIN`.

Things to know:

- A higher number is drawn on top of a lower one.
- Entities with the same number are drawn in entity ID order.
- Setting an entity's `ZIndex` `visible` flag to false skips it when drawing.