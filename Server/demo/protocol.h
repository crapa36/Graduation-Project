constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int MAX_USER = 10;
constexpr int MAX_NAME_SIZE = 20;



//ÆÐÅ¶ ID
constexpr char CS_IN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_ROTATION = 2;

constexpr char SC_REMOVE_PLAYER = 4;

struct SC_REMOVE_PLAYER_PACKET {
	unsigned char size;
	char	type;
	short	id;
};
