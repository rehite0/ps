#include <immintrin.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "collision.h"
#include "pse_const.h"

// #define DIVISIONS int
// #define COLL_COFFICIENT (0.95f)
#define BKT_NUM 1024
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct{ int x;int y;} Cell_id;
typedef struct{ BALL a;BALL b;} Pair;

int num_coll=0;
static BALL **bkt_list=NULL;
static int *bkt_len=NULL;
static int *bkt_size=NULL;



void collision_setup(void);
void collision_del(void);
void collision_reset(void);
void collision_register(BALL start,BALL stop);
int collision_detect();
static void resolve_logic_scalar(const Pair *pairs,int num_pair);
static void resolve_logic_simd(const Pair *pairs,int num_pair);
static inline u32 hash(Cell_id c);
static inline Cell_id get_id(float x,float y);
/**
 * @brief Scalar fallback for 2D elastic collision resolution across an array of pairs.
 * 
 * @param pairs           Array of ball index pairs
 * @param num_pair        Total number of pairs (handles 0 up to 256 or more)
 * @param buff            Struct-of-Arrays containing ball attributes
 * @param e               Coefficient of restitution (0.0 to 1.0)
 * @param fixed_bit_shift Bit index x (0-31) in flag which indicates an immovable ball
 */
static void resolve_logic_scalar(const Pair *pairs,int num_pair) {
    const float eps = 1e-8f;
    for (int i = 0; i < num_pair; ++i) {
        u32 ida = pairs[i].a;
        u32 idb = pairs[i].b;
	if(ball_buff.flag[ida] & NO_COLLISION) continue;
	if(ball_buff.flag[idb] & NO_COLLISION) continue;
        int fixed_a = (ball_buff.flag[ida] & NO_CONSTRAIN) != 0;
        int fixed_b = (ball_buff.flag[idb] & NO_CONSTRAIN) != 0;
        float inv_mass_a = fixed_a ? 0.0f : 1.0f;
        float inv_mass_b = fixed_b ? 0.0f : 1.0f;
        float inv_mass_sum = inv_mass_a + inv_mass_b;
        if (inv_mass_sum <= 0.0f) continue;
        float posx_a = ball_buff.posx[ida];
        float posy_a = ball_buff.posy[ida];
        float posx_b = ball_buff.posx[idb];
        float posy_b = ball_buff.posy[idb];

        float rad_a = ball_buff.rad[ida];
        float rad_b = ball_buff.rad[idb];

        float dx = posx_b - posx_a;
        float dy = posy_b - posy_a;
        float dist_sq = dx * dx + dy * dy;

        float min_dist = rad_a + rad_b;
        float min_dist_sq = min_dist * min_dist;

        if (dist_sq >= min_dist_sq) continue;

	if (dist_sq <= eps) {
	    dx = 1e-4f;
	    dy = 0.0f;
	    dist_sq = eps;
	}
        float dist = sqrtf(dist_sq);
        float inv_dist = 1.0f / dist;
        float nx = dx * inv_dist;
        float ny = dy * inv_dist;
        float overlap = min_dist - dist;
        float inv_mass_sum_recip = 1.0f / inv_mass_sum;
        float shift_a = overlap * (inv_mass_a * inv_mass_sum_recip);
        float shift_b = overlap * (inv_mass_b * inv_mass_sum_recip);
        float posx_a_new = posx_a - nx * shift_a;
        float posy_a_new = posy_a - ny * shift_a;
        float posx_b_new = posx_b + nx * shift_b;
        float posy_b_new = posy_b + ny * shift_b;
        float vx_a = posx_a - ball_buff.pposx[ida];
        float vy_a = posy_a - ball_buff.pposy[ida];
        float vx_b = posx_b - ball_buff.pposx[idb];
        float vy_b = posy_b - ball_buff.pposy[idb];
        float vrel_x = vx_b - vx_a;
        float vrel_y = vy_b - vy_a;
        float v_normal = vrel_x * nx + vrel_y * ny;
        float dvx_a = 0.0f, dvy_a = 0.0f;
        float dvx_b = 0.0f, dvy_b = 0.0f;
        if (v_normal < 0.0f) {
            float J = (-(1.0f + COLL_COFFICIENT) * v_normal) * inv_mass_sum_recip;
            float Jx = J * nx;
            float Jy = J * ny;

            dvx_a = -Jx * inv_mass_a;
            dvy_a = -Jy * inv_mass_a;
            dvx_b = Jx * inv_mass_b;
            dvy_b = Jy * inv_mass_b;
        }
        float pposx_a_new = posx_a_new - (vx_a + dvx_a);
        float pposy_a_new = posy_a_new - (vy_a + dvy_a);
        float pposx_b_new = posx_b_new - (vx_b + dvx_b);
        float pposy_b_new = posy_b_new - (vy_b + dvy_b);
        ball_buff.posx[ida] = posx_a_new;
        ball_buff.posy[ida] = posy_a_new;
        ball_buff.pposx[ida] = pposx_a_new;
        ball_buff.pposy[ida] = pposy_a_new;

        ball_buff.posx[idb] = posx_b_new;
        ball_buff.posy[idb] = posy_b_new;
        ball_buff.pposx[idb] = pposx_b_new;
        ball_buff.pposy[idb] = pposy_b_new;

	assert(!isnan(ball_buff.posx[ida]));
	assert(!isnan(ball_buff.posy[ida]));
	assert(!isinf(ball_buff.posx[ida]));
	assert(!isinf(ball_buff.posy[ida]));
	assert(!isnan(ball_buff.posx[idb]));
	assert(!isnan(ball_buff.posy[idb]));
	assert(!isinf(ball_buff.posx[idb]));
	assert(!isinf(ball_buff.posy[idb]));
	assert(!isnan(ball_buff.pposx[ida]));
	assert(!isnan(ball_buff.pposy[ida]));
	assert(!isinf(ball_buff.pposx[ida]));
	assert(!isinf(ball_buff.pposy[ida]));
	assert(!isnan(ball_buff.pposx[idb]));
	assert(!isnan(ball_buff.pposy[idb]));
	assert(!isinf(ball_buff.pposx[idb]));
	assert(!isinf(ball_buff.pposy[idb]));
    }
}
/**
 * @brief Resolves 2D elastic collisions across array of ball pairs using AVX-512.
 * 
 * @param pairs           Array of ball index pairs
 * @param num_pair        Total number of pairs (e.g. 256 or lower down to 0)
 * @param buff            Struct-of-Arrays containing ball attributes
 * @param e               Coefficient of restitution (0.0 to 1.0)
 * @param fixed_bit_shift Bit index x (0-31) in flag which indicates immovable ball
 */
static void resolve_logic_simd(const Pair *pairs, int num_pair) {

    // SIMD lane offset maps for pair layout: [idA_0, idB_0, idA_1, idB_1, ...]
    const __m512i v_offsets_A = _mm512_set_epi32(30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
    const __m512i v_offsets_B = _mm512_set_epi32(31, 29, 27, 25, 23, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1);

    const __m512 v_ones  = _mm512_set1_ps(1.0f);
    const __m512 v_zeros = _mm512_setzero_ps();
    const __m512 v_eps   = _mm512_set1_ps(1e-8f);
    const __m512 v_e     = _mm512_set1_ps(COLL_COFFICIENT);
    const __m512 v_half  = _mm512_set1_ps(0.5f);
    const __m512 v_three = _mm512_set1_ps(3.0f);

    const u32 bit_mask = NO_CONSTRAIN;
    const __m512i v_flag_bit = _mm512_set1_epi32((int)bit_mask);

    for (int i = 0; i < num_pair; i += 16) {
        // 1. Dynamic tail mask for arbitrary pair counts <= 256
        int remaining = num_pair - i;
        int active_count = (remaining < 16) ? remaining : 16;
        __mmask16 active_mask = (active_count == 16) ? 0xFFFF : (u16)((1U << active_count) - 1);

        const int *pair_ptr = (const int *)&pairs[i];

        // 2. Gather Pair IDs
        __m512i v_idA = _mm512_mask_i32gather_epi32(_mm512_setzero_si512(), active_mask, v_offsets_A, pair_ptr, 4);
        __m512i v_idB = _mm512_mask_i32gather_epi32(_mm512_setzero_si512(), active_mask, v_offsets_B, pair_ptr, 4);

        // 3. Gather Ball Attributes
        __m512 posxA  = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idA, ball_buff.posx, 4);
        __m512 posyA  = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idA, ball_buff.posy, 4);
        __m512 pposxA = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idA, ball_buff.pposx, 4);
        __m512 pposyA = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idA, ball_buff.pposy, 4);
        __m512 radA   = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idA, ball_buff.rad, 4);
        __m512i flagA = _mm512_mask_i32gather_epi32(_mm512_setzero_si512(), active_mask, v_idA, (const int *)ball_buff.flag, 4);

        __m512 posxB  = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idB, ball_buff.posx, 4);
        __m512 posyB  = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idB, ball_buff.posy, 4);
        __m512 pposxB = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idB, ball_buff.pposx, 4);
        __m512 pposyB = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idB, ball_buff.pposy, 4);
        __m512 radB   = _mm512_mask_i32gather_ps(v_zeros, active_mask, v_idB, ball_buff.rad, 4);
        __m512i flagB = _mm512_mask_i32gather_epi32(_mm512_setzero_si512(), active_mask, v_idB, (const int *)ball_buff.flag, 4);

        // 4. Determine Movability (Inverse Mass calculation)
        __mmask16 fixedA = _mm512_test_epi32_mask(flagA, v_flag_bit);
        __mmask16 fixedB = _mm512_test_epi32_mask(flagB, v_flag_bit);

        __m512 inv_massA = _mm512_mask_blend_ps(fixedA, v_ones, v_zeros);
        __m512 inv_massB = _mm512_mask_blend_ps(fixedB, v_ones, v_zeros);
        __m512 inv_mass_sum = _mm512_add_ps(inv_massA, inv_massB);

        // Skip lanes where both balls are fixed or lane is inactive
        __mmask16 process_mask = active_mask & _mm512_cmp_ps_mask(inv_mass_sum, v_zeros, _CMP_GT_OQ);

        // 5. Broad/Near-phase Distance Test
        __m512 dx = _mm512_sub_ps(posxB, posxA);
        __m512 dy = _mm512_sub_ps(posyB, posyA);
        __m512 dist_sq = _mm512_fmadd_ps(dx, dx, _mm512_mul_ps(dy, dy));

        __m512 min_dist = _mm512_add_ps(radA, radB);
        __m512 min_dist_sq = _mm512_mul_ps(min_dist, min_dist);

        // Mask of colliding lanes
        __mmask16 collide_mask = _mm512_mask_cmp_ps_mask(process_mask, dist_sq, min_dist_sq, _CMP_LT_OQ);
        if (collide_mask == 0) continue; // Early exit if no pair collides in this 16-batch

        // 6. Handle Coincident Centers Gracefully (zero distance)
        __mmask16 zero_mask = _mm512_mask_cmp_ps_mask(collide_mask, dist_sq, v_eps, _CMP_LE_OQ);
        dx = _mm512_mask_blend_ps(zero_mask, dx, min_dist);
        dy = _mm512_mask_blend_ps(zero_mask, dy, v_zeros);
        dist_sq = _mm512_mask_blend_ps(zero_mask, dist_sq, v_eps);

        // 7. Inverse Distance Computation (rsqrt14 + 1 Newton-Raphson iteration)
        __m512 inv_dist = _mm512_rsqrt14_ps(dist_sq);
        __m512 muls = _mm512_mul_ps(_mm512_mul_ps(dist_sq, inv_dist), inv_dist);
        inv_dist = _mm512_mul_ps(_mm512_mul_ps(v_half, inv_dist), _mm512_sub_ps(v_three, muls));
        __m512 dist = _mm512_mul_ps(dist_sq, inv_dist);

        // Normal unit vector
        __m512 nx = _mm512_mul_ps(dx, inv_dist);
        __m512 ny = _mm512_mul_ps(dy, inv_dist);

        // 8. Positional Anti-Overlap Separation
        __m512 overlap = _mm512_sub_ps(min_dist, dist);
        __m512 inv_mass_sum_recip = _mm512_div_ps(v_ones, inv_mass_sum);
        
        __m512 shiftA = _mm512_mul_ps(overlap, _mm512_mul_ps(inv_massA, inv_mass_sum_recip));
        __m512 shiftB = _mm512_mul_ps(overlap, _mm512_mul_ps(inv_massB, inv_mass_sum_recip));

        __m512 posxA_new = _mm512_fnmadd_ps(nx, shiftA, posxA);
        __m512 posyA_new = _mm512_fnmadd_ps(ny, shiftA, posyA);
        __m512 posxB_new = _mm512_fmadd_ps(nx, shiftB, posxB);
        __m512 posyB_new = _mm512_fmadd_ps(ny, shiftB, posyB);

        // 9. Derive Implicit Velocities & Elastic Impulse
        __m512 vxA = _mm512_sub_ps(posxA, pposxA);
        __m512 vyA = _mm512_sub_ps(posyA, pposyA);
        __m512 vxB = _mm512_sub_ps(posxB, pposxB);
        __m512 vyB = _mm512_sub_ps(posyB, pposyB);

        __m512 vrel_x = _mm512_sub_ps(vxB, vxA);
        __m512 vrel_y = _mm512_sub_ps(vyB, vyA);
        __m512 v_normal = _mm512_fmadd_ps(vrel_x, nx, _mm512_mul_ps(vrel_y, ny));

        // Only apply impulse if objects are approaching each other
        __mmask16 approach_mask = _mm512_mask_cmp_ps_mask(collide_mask, v_normal, v_zeros, _CMP_LT_OQ);

        __m512 one_plus_e = _mm512_add_ps(v_ones, v_e);
        __m512 J = _mm512_div_ps(_mm512_mul_ps(_mm512_sub_ps(v_zeros, one_plus_e), v_normal), inv_mass_sum);

        __m512 Jx = _mm512_mul_ps(J, nx);
        __m512 Jy = _mm512_mul_ps(J, ny);

        __m512 dvxA = _mm512_maskz_mov_ps(approach_mask, _mm512_sub_ps(v_zeros, _mm512_mul_ps(Jx, inv_massA)));
        __m512 dvyA = _mm512_maskz_mov_ps(approach_mask, _mm512_sub_ps(v_zeros, _mm512_mul_ps(Jy, inv_massA)));
        __m512 dvxB = _mm512_maskz_mov_ps(approach_mask, _mm512_mul_ps(Jx, inv_massB));
        __m512 dvyB = _mm512_maskz_mov_ps(approach_mask, _mm512_mul_ps(Jy, inv_massB));

        // 10. Compute Updated Velocity & Sync Previous Position ppos = pos_new - v_new
        __m512 pposxA_new = _mm512_sub_ps(posxA_new, _mm512_add_ps(vxA, dvxA));
        __m512 pposyA_new = _mm512_sub_ps(posyA_new, _mm512_add_ps(vyA, dvyA));
        __m512 pposxB_new = _mm512_sub_ps(posxB_new, _mm512_add_ps(vxB, dvxB));
        __m512 pposyB_new = _mm512_sub_ps(posyB_new, _mm512_add_ps(vyB, dvyB));

        // 11. Masked Scatter Updates Back to Buffer
        _mm512_mask_i32scatter_ps(ball_buff.posx,  collide_mask, v_idA, posxA_new,  4);
        _mm512_mask_i32scatter_ps(ball_buff.posy,  collide_mask, v_idA, posyA_new,  4);
        _mm512_mask_i32scatter_ps(ball_buff.pposx, collide_mask, v_idA, pposxA_new, 4);
        _mm512_mask_i32scatter_ps(ball_buff.pposy, collide_mask, v_idA, pposyA_new, 4);

        _mm512_mask_i32scatter_ps(ball_buff.posx,  collide_mask, v_idB, posxB_new,  4);
        _mm512_mask_i32scatter_ps(ball_buff.posy,  collide_mask, v_idB, posyB_new,  4);
        _mm512_mask_i32scatter_ps(ball_buff.pposx, collide_mask, v_idB, pposxB_new, 4);
        _mm512_mask_i32scatter_ps(ball_buff.pposy, collide_mask, v_idB, pposyB_new, 4);
    }
}

int collision_detect()
{
	int coll_count=0;
	int n=0;
	Pair arr[256];
	for(int c=0;c<BKT_NUM;++c){
		for(int i=0;i<bkt_len[c];++i){
			for(int j=i+1;j<bkt_len[c];++j){
				arr[n].a=bkt_list[c][i];
				arr[n].b=bkt_list[c][j];
				++n;
				if(n==255){
					resolve_logic_scalar(arr,n);
					n=0;
				}
			}
		}
		
	}
	if(n>0){
		(void)resolve_logic_simd;
		resolve_logic_scalar(arr,n);
		n=0;
	}
	return coll_count;
}

void collision_register(BALL start,BALL stop){
	for(BALL b=start;b<stop;++b){
		#if(log_coll_no)
		src_buff.coll_no[b]=0;
		#endif
		const Cell_id bl=get_id(
				ball_buff.posx[b]-ball_buff.rad[b],
				ball_buff.posy[b]-ball_buff.rad[b]
				),
			tr=get_id(
				ball_buff.posx[b]+ball_buff.rad[b],
				ball_buff.posy[b]+ball_buff.rad[b]
				);
		const int xo=bl.x, yo=bl.y, xm=tr.x, ym=tr.y;
		for(int i=xo;i<=xm;++i)
			for(int j=yo;j<=ym;++j){
				Cell_id id={i,j};
				u32 idx= hash( id ) %BKT_NUM;
				if(bkt_len[idx]>=bkt_size[idx]){
					bkt_size[idx]*=2;
					bkt_list[idx]=(BALL*)realloc(bkt_list[idx]
							,(size_t)bkt_size[idx]*sizeof(BALL));
				}
				bkt_list[idx][bkt_len[idx]]=b;
				++bkt_len[idx];
			}
	}
}
static u32 hash(Cell_id c){
	u64 key = ((u64)(u32)c.x << 32) | (u32)c.y;
	// Apply MurmurHash3 / SplitMix64 finalizer mixing constants
	key ^= key >> 30;
	key *= 0xbf58476d1ce4e5b9ULL;
	key ^= key >> 27;
	key *= 0x94d049bb133111ebULL;
	key ^= key >> 31;

	return (u32)(key^(key>>32));
}
static inline Cell_id get_id(float x,float y){
	Cell_id ret={
		(int)((x+2.0f)*(float)(DIVISIONS)/4.0f),
		(int)((y+2.0f)*(float)(DIVISIONS)/4.0f),
	};
	return ret;
}

void collision_setup(void){
	bkt_list=(BALL**)malloc(BKT_NUM*sizeof(BALL*));
	bkt_len=(int*)malloc(BKT_NUM*sizeof(int));
	bkt_size=(int*)malloc(BKT_NUM*sizeof(int));
	for(int i=0;i<BKT_NUM;++i){
		bkt_len[i]=0;
		bkt_size[i]=32;
		bkt_list[i]=malloc((size_t)bkt_size[i]*sizeof(BALL*));
	}
}
void collision_reset(void)
{
	for(int i=0;i<BKT_NUM;++i)
		bkt_len[i]=0;
}
void collision_del(void){
	for(int i=0;i<BKT_NUM;++i)
		free(bkt_list[i]);
	free(bkt_list);
	free(bkt_len);
	free(bkt_size);
}
