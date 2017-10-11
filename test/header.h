
//Global variables for arguments
extern int width;
extern int iq_size;
extern int rob_size;
extern int cache_size;
extern int p;
extern char * trace;

//pipeline registers
extern struct instruction * de;
extern struct instruction * rn;
extern struct instruction * rr;
extern struct instruction * di;
extern struct instruction * iq;
extern struct instruction * ex_list;
extern struct instruction * wb;
extern struct instruction * rob;

//ROB variables
extern int rob_head;
extern int rob_tail;

//Rename map table
extern int rmt[67];

//boolean variables
extern int pipeline_empty;
extern int trace_empty;

//counters
extern int cycle_count;
extern int instr_count;

//instruction cache variables
extern struct Block **cache;
extern int num_sets;
extern int tag_width;
extern int offset_width;
extern int index_width;
extern int c_hits;
extern int c_tag;
extern int c_index;
extern int c_pc;
extern int miss_timer;

//prefetch variables
extern int p_hits;

//function definitions
void retire();
void writeback();
void execute();
void issue();
void dispatch();
void regRead();
void rrename();
void decode();
void fetch();

int advance_cycle();

int deIsEmpty();
int rnIsEmpty();
int rrIsEmpty();
int diIsEmpty();
int iqIsEmpty();
int exIsEmpty();
int wbIsEmpty();
int robIsEmpty();

int cache_process();
void rank_LRU_hit();
void rank_LRU_miss_cold();
void rank_LRU_miss_replace();

//instruction definition
struct instruction {
  int pc;
  int instr_num;
  int op_type;
  int dest_reg;
  int dest_tag;
  int src1_reg;
  int src1_orig;
  int src2_reg;
  int src2_orig;
  int fe_beg, fe_dur, de_beg, de_dur, rn_beg, rn_dur, rr_beg, rr_dur, di_beg, di_dur, is_beg, is_dur, ex_beg, ex_dur, wb_beg, wb_dur, rt_beg, rt_dur;
  int iq_src1_ready;
  int iq_src2_ready;
  int iq_counter;
  int rob_ready;
  int timer;
  int empty;
};

struct Block {
  unsigned int tag;
  unsigned int LRURank;
  int isFull;
  int prefetch_flag;
  int prefetch_timer;
};

#define CYCLE_COUNT_MAX 100000

#define BLOCK_SIZE 64
#define NUM_WAYS 4
#define MISS_DELAY 10
#define PC_LENGTH 24
