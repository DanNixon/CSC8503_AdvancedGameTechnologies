# Client CLI Usage

- `connect [local port] [server IP] [server port]`

  Connects to a server instance at the given IP address and port.
  Supplying a unique local port allows multiple client instances on the same machine.

- `disconnect`

  Disconnects from the currently connected server.

- `publish [topic] [message]`

  Publishes a string message on a given topic.
  Topic must contain no spaces.

- `subscribe [topic]`

  Subscribes to string message on a given topic.
  Topic must contain no spaces.

- `physics pause [yes/no]`

  Sets the pause state of the physics simulation.

- `physics get [object name] [value name]`

  Gets a value for an object in the physics simulation.

- `physics set [object name] [value name] [value]`

  Sets a value for an object in the physics simulation.

- `physics collsub [object name]`

  Subscribes to collision events for a given object.

- `game announce [message]`

  Announces a message to the log in the game.

- `game highscores [count]`

  Gets the top highscores.

- `game savescores`

  Saves recorded scores to disk.

- `game loadscores`

  Loads recorded scores back from disk.

- `game ammogive [quantity]`

  Gives or takes ammo from the player.

- `game shotablelife [duration]`

  Sets the time in seconds that objects shot by the player exist before
  despawning.

- `game planetspeed [angular velocity]`

  Sets the speed of rotation of the planet.

- `game camspeed [speed]`

  Sets the movement speed of the currently active player camera.

## Physical value names

- `inv_mass` (float)
- `position` (3 element vector)
- `lin_vel` (3 element vector)
- `force` (3 element vector)
- `orientation` (quaternion)
- `ang_vel` (3 element vector)
- `torque` (3 element vector)

## Value formats

- Float: `0.1`
- 3 element vector: `[0.1,4.8,9.7]` (no spaces)
- Quaternion: `[0.1,0.3,0.4,1.0]` (no spaces)

## Some names of objects in the simulation:

- `planet`
- `target`
- `lamp_post`
- `space_cube`
- `space_sphere`
- `test_plane_1`
- `test_plane_2`

(`space_cube` and `space_sphere` start the simulation unbounded so are probably
best for using with the physics CLI)
