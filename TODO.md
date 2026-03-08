# Done #
- Functional two player snake game
- Server and client  (not hooked up yet)
- Serdes for game state
- Game logic handling for n snakes
- Thread safe queue for passing between network and game logic layers.

# TODO #
- Decouple the SnakeGameLogic required for the server from that required by the client
  - Server will require full game logic handling *but no rendering*
  - Client will require game logic to update it's own snake, and to apply snapshots from server, and to render.
- Add incoming message handling for server and client - deserialize and delegate to the correct snake game logic function.
- Add mechanisms to trigger outgoing messages at the end of the tick, and to populate outgoing messages
- Add registration mechanism.
