# Client CLI Usage

- `connect [local port] [server IP] [server port]`

  Connects to a server instance at the given IP address and port.
  Supplying a unique local port allows multiple client instances on the same machine.

- `disconnect`

  Disconnects from the currently connected server.

- `publish [topic] [message]`
- `subscribe [topic]`
- `physics pause [yes/no]`
- `physics get [object name] [value name]`
- `physics set [object name] [value name] [value]`
- `physics collsub [object name]`
- `game announce [message]`
- `game highscores [count]`
- `game savescores`
- `game loadscores`
- `game ammogive [quantity]`
- `game shotablelife [duration]`
- `game planetspeed [angular velocity]`
