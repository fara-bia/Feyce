WARNING: THIS PROJECT IS UNFINISHED YET, THIS README IS PRE-WRITTEN AND WON'T WORK YET

____________________________________________________________
This is the backend version of Feyce that works in terminal.

Initialization steps:
1. Download the executable file appropriate to your operating system.
2. Open your terminal (bash, powershell, cmd etc.), make it point to the folder in which you downloaded the executable.
3. Usage: ./Feyce < FEN Code > < Depth > < Greed (10.000 by default) >

_________________________________
Feyce ChessBot Working Mechanism:

Preprocessing:
Decode FEN and save positions, castling, en passant

Board Array Items:
{0} = Turn White/Black (1/0)

{1, 64} = Pieces (goes from 8A to 1H)

{65, 66} = White Castling Availability (1/0, 1/0)

{67, 68} = Black Castling Availability

{69, 77} White Pawns En Passant (2, 1, 0 = Available, Not Available, No Piece)

{78, 86} Black Pawns En Passant Availability
Piece representations are identical to FEN (k, K, n, Q etc.)

Process:

The program runs for once.

Evaluate board according to: material value, development, proximity of controlled squares to the king itself;
- Material Value: 1 for pawns; 3 for bishops and knights; 5 for rooks; 9 for the queen; <greed> for the king. (overall standard values)
- Development: For pawns, the farthest square they can get. For knights, closeness to the center. For queen and closeness to the opponent's king. For bishops, closeness to their side's corners. For king, the closer side it'll castle to. 
- Proximity: Simply promixity of their closest controlled square are to the opponent's king.

Engine calculates each possible move of a side, then each possible responses opponent can give. 1 Depth is doing this for once. As depth increases, machine keeps calculating further possible positions. 

It finally chooses the optimal move that'll give highest change in value it'll get in the end, compared to current board. Even if it's negative.

(Engine uses static memory, since argument vectors decide parameters.)
