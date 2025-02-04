//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// BYTE ORDER & ALIGNMENT
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif

#ifndef IWORDS_MUST_ALIGN
	#if defined(__i386__) || defined(__i386) || defined(_i386_)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(_M_IX86) || defined(_X86_) || defined(__x86_64__)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(__amd64) || defined(__amd64__)
		#define IWORDS_MUST_ALIGN 0
	#else
		#define IWORDS_MUST_ALIGN 1
	#endif
#endif


// kcp包，kcp通信最小单位，有4种类型（见cmd字段）
//=====================================================================
// SEGMENT
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node; // 用于将该 Segment 链入队列
	// 以下字段会被编码 除去data有24字节
    IUINT32 conv;           // 会话标识符（Conversation ID）
    IUINT32 cmd;            // 命令类型，4种类型，data包：应用数据   ack包：告诉发送方收到某个data包   询问对方的接收窗口大小    告诉对方自己的接收窗口大小
    IUINT32 frg;            // 分片编号，应用消息长度超过设置的kcp->mss会分片，编号从大到小，最后一个为0，表示最后一个分片
    IUINT32 wnd;            // 自己的（剩余）接收窗口大小，所有类型（4种）的包都有该字段值
    IUINT32 ts;             // 发送时间戳，data类型：每次发送（包括重传）都是当前发送时间；ack类型：依然是发送的时间，用来计算rtt
    IUINT32 sn;             // 序号，data类型：来自kcp->snd_nxt自增； ack类型：确认收到的data包序号；只为data包服务
    IUINT32 una;            // 下一个按序接收的序号，所有类型（4种）的包都有该字段值，值为 kcp->rcv_nxt, 告诉发送方 sn < una 的包都已收到
    IUINT32 len;            // 数据部分的长度，仅仅data包有效
	char data[1];           // 数据缓冲区，仅仅data包有效
	// 以下字段不会被编码
    IUINT32 resendts;       // 下次重传时间戳（Resend Timestamp），在flush中判断超时重传，根据 rto 字段计算
    IUINT32 rto;            // 重传超时（Retransmission Timeout），这是时间段，首次发送时由 kcp->rx_rto 赋值, 重传时根据nodelay模式选择不同公式增长（详细见代码）
    IUINT32 fastack;        // 收到ack包时计算的该分片被跳过的累计次数 用于快速重传判断（ fastack > kcp->fastresend ）；快速：没有等到超时的情况下就重传
    IUINT32 xmit;           // 累计发送次数（Transmit Attempts），包括重传
};


//---------------------------------------------------------------------
// IKCPCB
//---------------------------------------------------------------------
struct IKCPCB
{
	// 基本信息
    IUINT32 conv;     // 会话标识符（conversation ID）
    IUINT32 mtu;      // 最大传输单元（Maximum Transmission Unit）, kcp包 segment 的最长大小
    IUINT32 mss;      // 最大分段大小（Maximum Segment Size），应用数据的分片大小，等于 mtu 减去包头（24字节）开销

	// 状态信息
    IUINT32 state;    // 连接状态，0正常，-1网络差(存在seg.xmit >= dead_link)
	IUINT32 dead_link;// 网络不可用的阈值，仅仅在初始化设置

    // 发送控制
    IUINT32 snd_una;  // 第一个已发送 未确认的包序号，snd_buf队列中最小（最前面）的seg.sn； ikcp_input 里处理完una、ack会更新该字段
    IUINT32 snd_nxt;  // 自增的segment序号，等同于已发送的最大包序号
    IUINT32 rcv_nxt;  // 下一个期望接收的包序号，自增，表示 sn < rcv_nxt 的包都已经收到，并从 rcv_buf 移入到了 rcv_queue

    // rtt 和 rto，以下3个状态都是 ikcp_input 里收到ack包时更新
    IUINT32 rx_rttval;  // RTT 的偏差 
    IUINT32 rx_srtt;    // 平滑往返时间（Smoothed Round Trip Time）
    IUINT32 rx_rto;     // 超时重传时间（Retransmission Timeout）, 根据rx_rttval和rx_srtt更新，赋值于首次发送的seg.rto
    IUINT32 rx_minrto;  // 最小重传超时时间，初始化和ikcp_nodelay里设置，用于计算 rx_rto 

	// 流量控制和拥塞控制 4个窗口状态
	IUINT32 snd_wnd;   	// 开发者设置的发送窗口大小，在初始化和ikcp_wndsize设置，用于计算 自己的发送窗口大小 = min(snd_wnd，rmt_wnd，cwnd)
	IUINT32 rcv_wnd;	// 开发者设置的接收窗口大小，在初始化和ikcp_wndsize设置，用于计算 自己的接收窗口大小 = rcv_wnd - nrcv_que
	IUINT32 rmt_wnd;	// 对方的接收窗口大小，仅仅在收到对方的seg.wnd时更新
    IUINT32 cwnd;      	// 拥塞窗口大小
    IUINT32 incr;      	// 可发送的字节数，用于更新 cwnd
	IUINT32 ssthresh;   // 慢启动门限（Slow Start Threshold），用于更新 cwnd
	int nocwnd;			// 是否 关闭使用拥塞窗口
	IUINT32 probe;		// 探查接收窗口大小标识，IKCP_ASK_TELL：告知远端窗口大小，IKCP_ASK_SEND：请求远端告知窗口大小 
	IUINT32 ts_probe;	// 下次探查接收窗口的时间戳
	IUINT32 probe_wait; // 探查窗口需要等待的时间

    // 4个队列
   	struct IQUEUEHEAD snd_queue; // 待发送的数据队列，上层应用发送的数据
    struct IQUEUEHEAD rcv_queue; // 按序，已收到，提供给上层应用读取的包队列（还未合并分片），影响接收窗口大小，长度为 nrcv_que， rcv_wnd - nrcv_que = 自己的接收窗口
    struct IQUEUEHEAD snd_buf;   // 发送缓冲区，都是data包，来自snd_queue，已发送（udp sendto）但还未确认；
    struct IQUEUEHEAD rcv_buf;   // 接收缓冲区，输入为下层协议的数据（ikcp_input里处理），输出为上层协议数据 rcv_queue 
    IUINT32 nrcv_buf;  // rcv_buf的包数量
    IUINT32 nsnd_buf;  // snd_buf的数量
    IUINT32 nrcv_que;  // rcv_queue的包数量    rcv_wnd - nrcv_que = 自己的接收窗口
    IUINT32 nsnd_que;  // snd_queue的数量

    // 时间控制
    IUINT32 current;   // 当前时间戳，毫秒, ikcp_update 里更新该字段
    IUINT32 interval;  // 用于更新ts_flush，是ikcp_flush刷新间隔 对系统循环效率有非常重要影响，在初始化、ikcp_interval、ikcp_nodelay设置
    IUINT32 ts_flush;  // 下次flush刷新时间戳
    IUINT32 xmit;      // 会话期间累计发送包的次数，代码里只更新并没有使用过

	IUINT32 ts_recent;  // 代码里未使用
    IUINT32 ts_lastack; // 代码里未使用

    // 配置选项
    IUINT32 updated;   // 是否已更新标志
    IUINT32 nodelay;   // 是否启用无延迟模式 
 
	// 待发送的ack列表
	IUINT32 *acklist; // 待发送的ack列表，在 ikcp_input 处理 data 类型包时加入
	IUINT32 ackcount; // acklist中ack的数量 每个ack在acklist中存储sn ts两个量
	IUINT32 ackblock; // acklist的内存容量 2的倍数

	int fastresend; // 触发快速重传的次数，阈值，0表示关闭快速重传，在初始化和 ikcp_nodelay 设置； seg.fastack >= fastresend
	int fastlimit;  // 快速重传的限制，上限，仅仅在ikcp初始化的时候设置； seg.xmit <= fastlimit 

	void *user;
	char *buffer; // 写入到fd之前的缓存，满了的话会调用 kcp->output 写入fd；也就是说，调用一次 kcp->output （udp sendto函数）会批量写入多个segment
	
	int stream; // 是否采用流传输模式
	int logmask;
	int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
	void (*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// interface
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


