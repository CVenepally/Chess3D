Chess 3D

Controls

- '~' to open DevConsole. At this moment, DevConsole commands ARE case sensitive.
	- Use ChessBegin to restart a game.
	- Use ChessMove from=[letter][rank] to=[letter][rank] to move a piece. Letters between a-h (lower case only for now), rank from 1-8.
		- Example: ChessMove from=a1 to=a3

- F1 to DebugRender. Displays World Origin, Board tiles as Rows and Columns (Ex: (1, 1)) and also Chess square indexes (don't know if that is the right term) (Ex: (B, 2))
- F4 to Toggle FreeCamera. Default is Off/Auto
- Arrow Keys - Change Directional Light direction

0 - Lit with normal maps
1 - Diffuse Texel
2 - Normal Texel
3 - UVs
4 - Tangent (world space)
5 - Bitangent (world space)
6 - Normal (world space)
7 - Normal map normals
8 - Light Strength w/ normal maps