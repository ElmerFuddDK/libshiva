/* data type for UUID generator persistent state */
typedef struct {
    uuid_time_t  ts;       /* saved timestamp */
    uuid_node_t  node;     /* saved node ID */
    unsigned16   cs;       /* saved clock sequence */
} uuid_state;

typedef struct {
     char get_ieee_node_identifier_inited;
     uuid_node_t get_ieee_node_identifier_saved_node;

	 char state_inited;
	 uuid_state st;

	 char current_time_inited;
	 uuid_time_t current_time_last;
     unsigned16 current_time_uuids_this_tick;
} uuid_globals_t;

#define GLOB uuid_globals_t* g = (uuid_globals_t*)globals
