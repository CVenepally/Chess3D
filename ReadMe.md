# Chess 3D

## How To Play

- Left Click to choose a piece to move / choose a tile to place held piece.
- Right Click to leave selected piece.

- '~' to open DevConsole. At this moment, DevConsole commands ARE case sensitive.
	- Use ChessBegin to restart a game.
	- Use ChessMove from=[letter][rank] to=[letter][rank] to move a piece. Letters between a-h (lower case only for now), rank from 1-8.
		- Example: ChessMove from=a1 to=a3

- F1 to DebugRender. Displays World Origin, Board tiles as Rows and Columns (Ex: (1, 1)) and also Chess square indexes (don't know if that is the right term) (Ex: (B, 2))
- F4 to Toggle FreeCamera. Default is Off/Auto
- Arrow Keys - Change Directional Light direction

List of Chess Commands:

ChessListen
ChessConnect [ip=<ipAddress>][port=<portNumber>] (Defaults: ip=127.0.0.1 port=3100)
ChessServerInfo [ip=<ipAddress>][port=<portNumber>] (Defaults: ip=127.0.0.1 port=3100)
ChessPlayerInfo [name=<name>]
ChessBegin [firstPlayer=<name>] (Default name is caller name)
ChessMove [from=<fromSquare>][to=<toSquare>][promote=<q/r/n/b>]
ChessOfferDraw
ChessAcceptDraw
ChessRejectDraw
ChessResign
ChessValidate
ChessDisconnect [reason=<reason>]

Shader Debug Modes

0 - Lit (Default)
1 - Albedo
2 - Normal Map
3 - Roughness Map
4 - Metal Maps
5 - Ambient Occlusion Map
6 - UVs
7 - Vertex Normal [Raw]
8 - Vertex Tangents [Raw]
9 - Vertex Bitangents [Raw]
10 - Vertex Normal [Transformed]
11 - Vertex Tangents [Transformed]
12 - Vertex Bitangents [Transformed]
13 - Vertex Color
14 - Model Tint