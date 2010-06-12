# Technical overview #

## Adaptor objects ##

The meat of the bridge is the `QSpiAdaptor` and `QSpiObject` classes. These
classes adapt method calls from the AT-SPI interface to the `QAccessible`
interface. For every widget that Qt makes accessible using `QAccessible` a
`QSpiAdaptor` is created.

`QSpiObject` is the base class for `QSpiAdaptor` it takes care of basic methods
neccessary to implement the AT-SPI interface. Methods in this class are
neccessary, but not exposed in the AT-SPI protocol. Examples include
`getParentReference` and `getItem`. These methods get an object representing
a D-Bus reference for the parent widget, and get an object representing the
cacheable properties of the object respectively.

`QSpiAdaptor` has public methods that represent the entire exposed interface of
AT-SPI. `QSpiAdaptor` is NOT a D-Bus adaptor. D-Bus adaptors for each AT-SPI
interface are attached to `QSpiAdaptor` objects during construction.

`QSpiAccessible` and `QSpiApplication` are both subclasses of `QSpiAdaptor`.
A `QSpiAccessible` object is created for every widget exposed via AT-SPI
*except* the root widget. The root widget is handled using `QSpiApplication`.
These classes differ only in the implementation of `getParentReference` and
in which D-Bus adaptors they attach.

## Cache object ##

The cache object is responsible for:

- Listening to the `QSpiAccessible` interface and creating `QSpiAccessible`
  objects when new widgets are added.
- Listening to the `objectDestroyed` signal of these widgets and deleting
  its `QSpiAccessible` object on widged destruction. (Essentially tying the
  life-time of an adaptor object to the widget it represents.
- Maintaining a look-up mechanism to get hold of a `QSpiAccessible` from the
  widget it represents.
- Maintaining a list of all the existing `QSpiAccessible` objects.

## Runtime ##

On initializing the plugin the `QSpiAccessibleBridge` class creates a single
`QSpiCache` object and creates a D-Bus adaptor for this class. The D-Bus
adaptor implements the 'Cache' interface of AT-SPI using methods on the
cache object. The cache is then responsible for creating a `QSpiAccessible`
object for every `QAccessible` widget.
