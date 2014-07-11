/*
 * Copyright (c) 2003, 2007-14 Matteo Frigo
 * Copyright (c) 2003, 2007-14 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Tue Mar  4 13:51:49 EST 2014 */

#include "codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_hc2cdft_c.native -fma -reorder-insns -schedule-for-pipeline -simd -compact -variables 4 -pipeline-latency 8 -trivial-stores -variables 32 -no-generate-bytw -n 20 -dif -sign 1 -name hc2cbdftv_20 -include hc2cbv.h */

/*
 * This function contains 143 FP additions, 108 FP multiplications,
 * (or, 77 additions, 42 multiplications, 66 fused multiply/add),
 * 134 stack variables, 4 constants, and 40 memory accesses
 */
#include "hc2cbv.h"

static void hc2cbdftv_20(R *Rp, R *Ip, R *Rm, R *Im, const R *W, stride rs, INT mb, INT me, INT ms)
{
     DVK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DVK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DVK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DVK(KP618033988, +0.618033988749894848204586834365638117720309180);
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * ((TWVL / VL) * 38)); m < me; m = m + VL, Rp = Rp + (VL * ms), Ip = Ip + (VL * ms), Rm = Rm - (VL * ms), Im = Im - (VL * ms), W = W + (TWVL * 38), MAKE_VOLATILE_STRIDE(80, rs)) {
	       V T1M, T1T, T4, TF, T12, Te, T16, Ts, Tb, TN, TA, TG, TU, T1Y, T11;
	       V T1e, T29, T21, T15, Th, T13, Tp;
	       {
		    V TS, TT, Tf, T10, T20, T1Z, TX, Tg, Tn, To, T2, T3, TD, TE, T8;
		    V TV, T7, TZ, Tz, T9, Tu, Tv, T5, T6, Tx, Ty, Tc, Td, Tq, Tr;
		    V TY, Ta, TW, Tw;
		    T2 = LD(&(Rp[0]), ms, &(Rp[0]));
		    T3 = LD(&(Rm[WS(rs, 9)]), -ms, &(Rm[WS(rs, 1)]));
		    TD = LD(&(Rp[WS(rs, 5)]), ms, &(Rp[WS(rs, 1)]));
		    TE = LD(&(Rm[WS(rs, 4)]), -ms, &(Rm[0]));
		    T5 = LD(&(Rp[WS(rs, 4)]), ms, &(Rp[0]));
		    T6 = LD(&(Rm[WS(rs, 5)]), -ms, &(Rm[WS(rs, 1)]));
		    Tx = LD(&(Rp[WS(rs, 1)]), ms, &(Rp[WS(rs, 1)]));
		    Ty = LD(&(Rm[WS(rs, 8)]), -ms, &(Rm[0]));
		    T8 = LD(&(Rp[WS(rs, 6)]), ms, &(Rp[0]));
		    TS = VFMACONJ(T3, T2);
		    T4 = VFNMSCONJ(T3, T2);
		    TT = VFMACONJ(TE, TD);
		    TF = VFNMSCONJ(TE, TD);
		    TV = VFMACONJ(T6, T5);
		    T7 = VFNMSCONJ(T6, T5);
		    TZ = VFMACONJ(Ty, Tx);
		    Tz = VFNMSCONJ(Ty, Tx);
		    T9 = LD(&(Rm[WS(rs, 3)]), -ms, &(Rm[WS(rs, 1)]));
		    Tu = LD(&(Rp[WS(rs, 9)]), ms, &(Rp[WS(rs, 1)]));
		    Tv = LD(&(Rm[0]), -ms, &(Rm[0]));
		    Tc = LD(&(Rp[WS(rs, 8)]), ms, &(Rp[0]));
		    Td = LD(&(Rm[WS(rs, 1)]), -ms, &(Rm[WS(rs, 1)]));
		    Tq = LD(&(Rp[WS(rs, 7)]), ms, &(Rp[WS(rs, 1)]));
		    Tr = LD(&(Rm[WS(rs, 2)]), -ms, &(Rm[0]));
		    Tf = LD(&(Rp[WS(rs, 2)]), ms, &(Rp[0]));
		    TY = VFMACONJ(T9, T8);
		    Ta = VFMSCONJ(T9, T8);
		    TW = VFMACONJ(Tv, Tu);
		    Tw = VFNMSCONJ(Tv, Tu);
		    T12 = VFMACONJ(Td, Tc);
		    Te = VFNMSCONJ(Td, Tc);
		    T16 = VFMACONJ(Tr, Tq);
		    Ts = VFMSCONJ(Tr, Tq);
		    T10 = VSUB(TY, TZ);
		    T20 = VADD(TY, TZ);
		    Tb = VADD(T7, Ta);
		    TN = VSUB(T7, Ta);
		    T1Z = VADD(TV, TW);
		    TX = VSUB(TV, TW);
		    TA = VSUB(Tw, Tz);
		    TG = VADD(Tw, Tz);
		    Tg = LD(&(Rm[WS(rs, 7)]), -ms, &(Rm[WS(rs, 1)]));
		    Tn = LD(&(Rp[WS(rs, 3)]), ms, &(Rp[WS(rs, 1)]));
		    To = LD(&(Rm[WS(rs, 6)]), -ms, &(Rm[0]));
		    TU = VSUB(TS, TT);
		    T1Y = VADD(TS, TT);
		    T11 = VADD(TX, T10);
		    T1e = VSUB(TX, T10);
		    T29 = VSUB(T1Z, T20);
		    T21 = VADD(T1Z, T20);
		    T15 = VFMACONJ(Tg, Tf);
		    Th = VFMSCONJ(Tg, Tf);
		    T13 = VFMACONJ(To, Tn);
		    Tp = VFMSCONJ(To, Tn);
	       }
	       {
		    V T1S, T2B, T1W, T1I, T2q, T2w, T2i, T2c, T1C, T1K, T1s, T1g, T1, T2t, T1v;
		    V T1Q, T2A, T1q, T2m, TC, T1w, TP, T1x, T2f, T2r, T2g, T1E, T1D, T2y, T2x;
		    V T1i, T1h, T2D, T2C, T2s, T1t, T1u, T1y, T2u, TQ, T2d, T2e, T1U, T1L, T2j;
		    V T2k;
		    {
			 V T1R, T1F, T1V, T1o, TO, Tl, T1d, T2a, T1l, TB, TK, T1G, Tk, T1b, T19;
			 V T27, T25, T1H, TJ, T17, T23, TM, Ti, T14, T22, Tt, TH, Tj, T18, T24;
			 V TI, T2b, T2p, T1X, T2v, T2h, T2n, T1B, T1f, T28, T2o, T1a, TR, T1J, T1r;
			 V T1z, T26, Tm, TL, T1O, T1m, T1j, T2z, T1N, T1p, T1P, T2l, T1c, T1A, T1n;
			 V T1k;
			 T1R = LDW(&(W[TWVL * 18]));
			 T17 = VSUB(T15, T16);
			 T23 = VADD(T15, T16);
			 TM = VSUB(Te, Th);
			 Ti = VADD(Te, Th);
			 T14 = VSUB(T12, T13);
			 T22 = VADD(T12, T13);
			 Tt = VSUB(Tp, Ts);
			 TH = VADD(Tp, Ts);
			 T1F = LDW(&(W[TWVL * 28]));
			 T1V = LDW(&(W[TWVL * 8]));
			 T1o = VFMA(LDK(KP618033988), TM, TN);
			 TO = VFNMS(LDK(KP618033988), TN, TM);
			 Tj = VADD(Tb, Ti);
			 Tl = VSUB(Tb, Ti);
			 T18 = VADD(T14, T17);
			 T1d = VSUB(T14, T17);
			 T24 = VADD(T22, T23);
			 T2a = VSUB(T22, T23);
			 T1l = VFMA(LDK(KP618033988), Tt, TA);
			 TB = VFNMS(LDK(KP618033988), TA, Tt);
			 TI = VADD(TG, TH);
			 TK = VSUB(TG, TH);
			 T1G = VADD(T4, Tj);
			 Tk = VFNMS(LDK(KP250000000), Tj, T4);
			 T1b = VSUB(T11, T18);
			 T19 = VADD(T11, T18);
			 T27 = VSUB(T21, T24);
			 T25 = VADD(T21, T24);
			 T1H = VADD(TF, TI);
			 TJ = VFNMS(LDK(KP250000000), TI, TF);
			 T2b = VMUL(LDK(KP951056516), VFMA(LDK(KP618033988), T2a, T29));
			 T2p = VMUL(LDK(KP951056516), VFNMS(LDK(KP618033988), T29, T2a));
			 T1X = LDW(&(W[TWVL * 6]));
			 T1S = VZMUL(T1R, VADD(TU, T19));
			 T2v = LDW(&(W[TWVL * 22]));
			 T2B = VADD(T1Y, T25);
			 T26 = VFNMS(LDK(KP250000000), T25, T1Y);
			 T1W = VZMULI(T1V, VFMAI(T1H, T1G));
			 T1I = VZMULI(T1F, VFNMSI(T1H, T1G));
			 T2h = LDW(&(W[TWVL * 30]));
			 T2n = LDW(&(W[TWVL * 14]));
			 T1B = VMUL(LDK(KP951056516), VFMA(LDK(KP618033988), T1d, T1e));
			 T1f = VMUL(LDK(KP951056516), VFNMS(LDK(KP618033988), T1e, T1d));
			 T28 = VFMA(LDK(KP559016994), T27, T26);
			 T2o = VFNMS(LDK(KP559016994), T27, T26);
			 T1a = VFNMS(LDK(KP250000000), T19, TU);
			 TR = LDW(&(W[TWVL * 2]));
			 T1J = LDW(&(W[TWVL * 26]));
			 T1r = LDW(&(W[TWVL * 34]));
			 T1z = LDW(&(W[TWVL * 10]));
			 T1k = VFMA(LDK(KP559016994), Tl, Tk);
			 Tm = VFNMS(LDK(KP559016994), Tl, Tk);
			 T2q = VZMUL(T2n, VFMAI(T2p, T2o));
			 T2w = VZMUL(T2v, VFNMSI(T2p, T2o));
			 T2i = VZMUL(T2h, VFMAI(T2b, T28));
			 T2c = VZMUL(T1X, VFNMSI(T2b, T28));
			 T1c = VFNMS(LDK(KP559016994), T1b, T1a);
			 T1A = VFMA(LDK(KP559016994), T1b, T1a);
			 TL = VFNMS(LDK(KP559016994), TK, TJ);
			 T1n = VFMA(LDK(KP559016994), TK, TJ);
			 T1O = VFMA(LDK(KP951056516), T1l, T1k);
			 T1m = VFNMS(LDK(KP951056516), T1l, T1k);
			 T1j = LDW(&(W[TWVL * 36]));
			 T2z = LDW(&(W[0]));
			 T1N = LDW(&(W[TWVL * 20]));
			 T1C = VZMUL(T1z, VFMAI(T1B, T1A));
			 T1K = VZMUL(T1J, VFNMSI(T1B, T1A));
			 T1s = VZMUL(T1r, VFMAI(T1f, T1c));
			 T1g = VZMUL(TR, VFNMSI(T1f, T1c));
			 T1p = VFMA(LDK(KP951056516), T1o, T1n);
			 T1P = VFNMS(LDK(KP951056516), T1o, T1n);
			 T2l = LDW(&(W[TWVL * 16]));
			 T1 = LDW(&(W[TWVL * 4]));
			 T2t = LDW(&(W[TWVL * 24]));
			 T1v = LDW(&(W[TWVL * 12]));
			 T1Q = VZMULI(T1N, VFNMSI(T1P, T1O));
			 T2A = VZMULI(T2z, VFMAI(T1p, T1m));
			 T1q = VZMULI(T1j, VFNMSI(T1p, T1m));
			 T2m = VZMULI(T2l, VFMAI(T1P, T1O));
			 TC = VFMA(LDK(KP951056516), TB, Tm);
			 T1w = VFNMS(LDK(KP951056516), TB, Tm);
			 TP = VFNMS(LDK(KP951056516), TO, TL);
			 T1x = VFMA(LDK(KP951056516), TO, TL);
			 T2f = LDW(&(W[TWVL * 32]));
		    }
		    T2D = VCONJ(VSUB(T2B, T2A));
		    T2C = VADD(T2A, T2B);
		    T2s = VCONJ(VSUB(T2q, T2m));
		    T2r = VADD(T2m, T2q);
		    T1t = VADD(T1q, T1s);
		    T1u = VCONJ(VSUB(T1s, T1q));
		    T1y = VZMULI(T1v, VFNMSI(T1x, T1w));
		    T2u = VZMULI(T2t, VFMAI(T1x, T1w));
		    TQ = VZMULI(T1, VFNMSI(TP, TC));
		    T2g = VZMULI(T2f, VFMAI(TP, TC));
		    ST(&(Rm[0]), T2D, -ms, &(Rm[0]));
		    ST(&(Rp[0]), T2C, ms, &(Rp[0]));
		    ST(&(Rm[WS(rs, 4)]), T2s, -ms, &(Rm[0]));
		    ST(&(Rm[WS(rs, 9)]), T1u, -ms, &(Rm[WS(rs, 1)]));
		    T1E = VCONJ(VSUB(T1C, T1y));
		    T1D = VADD(T1y, T1C);
		    T2y = VCONJ(VSUB(T2w, T2u));
		    T2x = VADD(T2u, T2w);
		    T1i = VCONJ(VSUB(T1g, TQ));
		    T1h = VADD(TQ, T1g);
		    ST(&(Rp[WS(rs, 9)]), T1t, ms, &(Rp[WS(rs, 1)]));
		    T1L = VADD(T1I, T1K);
		    T1M = VCONJ(VSUB(T1K, T1I));
		    ST(&(Rp[WS(rs, 3)]), T1D, ms, &(Rp[WS(rs, 1)]));
		    ST(&(Rm[WS(rs, 6)]), T2y, -ms, &(Rm[0]));
		    ST(&(Rp[WS(rs, 6)]), T2x, ms, &(Rp[0]));
		    ST(&(Rm[WS(rs, 1)]), T1i, -ms, &(Rm[WS(rs, 1)]));
		    ST(&(Rp[WS(rs, 1)]), T1h, ms, &(Rp[WS(rs, 1)]));
		    T2d = VADD(T1W, T2c);
		    T2e = VCONJ(VSUB(T2c, T1W));
		    ST(&(Rm[WS(rs, 3)]), T1E, -ms, &(Rm[WS(rs, 1)]));
		    ST(&(Rp[WS(rs, 7)]), T1L, ms, &(Rp[WS(rs, 1)]));
		    T1U = VCONJ(VSUB(T1S, T1Q));
		    T1T = VADD(T1Q, T1S);
		    T2j = VADD(T2g, T2i);
		    T2k = VCONJ(VSUB(T2i, T2g));
		    ST(&(Rp[WS(rs, 2)]), T2d, ms, &(Rp[0]));
		    ST(&(Rp[WS(rs, 4)]), T2r, ms, &(Rp[0]));
		    ST(&(Rm[WS(rs, 5)]), T1U, -ms, &(Rm[WS(rs, 1)]));
		    ST(&(Rm[WS(rs, 2)]), T2e, -ms, &(Rm[0]));
		    ST(&(Rp[WS(rs, 8)]), T2j, ms, &(Rp[0]));
		    ST(&(Rm[WS(rs, 8)]), T2k, -ms, &(Rm[0]));
	       }
	       ST(&(Rp[WS(rs, 5)]), T1T, ms, &(Rp[WS(rs, 1)]));
	       ST(&(Rm[WS(rs, 7)]), T1M, -ms, &(Rm[WS(rs, 1)]));
	  }
     }
     VLEAVE();
}

static const tw_instr twinstr[] = {
     VTW(1, 1),
     VTW(1, 2),
     VTW(1, 3),
     VTW(1, 4),
     VTW(1, 5),
     VTW(1, 6),
     VTW(1, 7),
     VTW(1, 8),
     VTW(1, 9),
     VTW(1, 10),
     VTW(1, 11),
     VTW(1, 12),
     VTW(1, 13),
     VTW(1, 14),
     VTW(1, 15),
     VTW(1, 16),
     VTW(1, 17),
     VTW(1, 18),
     VTW(1, 19),
     {TW_NEXT, VL, 0}
};

static const hc2c_desc desc = { 20, XSIMD_STRING("hc2cbdftv_20"), twinstr, &GENUS, {77, 42, 66, 0} };

void XSIMD(codelet_hc2cbdftv_20) (planner *p) {
     X(khc2c_register) (p, hc2cbdftv_20, &desc, HC2C_VIA_DFT);
}
#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_hc2cdft_c.native -simd -compact -variables 4 -pipeline-latency 8 -trivial-stores -variables 32 -no-generate-bytw -n 20 -dif -sign 1 -name hc2cbdftv_20 -include hc2cbv.h */

/*
 * This function contains 143 FP additions, 62 FP multiplications,
 * (or, 131 additions, 50 multiplications, 12 fused multiply/add),
 * 114 stack variables, 4 constants, and 40 memory accesses
 */
#include "hc2cbv.h"

static void hc2cbdftv_20(R *Rp, R *Ip, R *Rm, R *Im, const R *W, stride rs, INT mb, INT me, INT ms)
{
     DVK(KP250000000, +0.250000000000000000000000000000000000000000000);
     DVK(KP559016994, +0.559016994374947424102293417182819058860154590);
     DVK(KP951056516, +0.951056516295153572116439333379382143405698634);
     DVK(KP587785252, +0.587785252292473129168705954639072768597652438);
     {
	  INT m;
	  for (m = mb, W = W + ((mb - 1) * ((TWVL / VL) * 38)); m < me; m = m + VL, Rp = Rp + (VL * ms), Ip = Ip + (VL * ms), Rm = Rm - (VL * ms), Im = Im - (VL * ms), W = W + (TWVL * 38), MAKE_VOLATILE_STRIDE(80, rs)) {
	       V TK, T1v, TY, T1x, T1j, T2f, TS, TT, TO, TU, T5, To, Tp, Tq, T2a;
	       V T2d, T2g, T2k, T2j, T1k, T1l, T18, T1m, T1f;
	       {
		    V T2, TP, T4, TR, TI, T1d, T9, T12, Td, T15, TE, T1a, Tv, T13, Tm;
		    V T1c, Tz, T16, Ti, T19, T3, TQ, TH, TG, TF, T6, T8, T7, Tc, Tb;
		    V Ta, TD, TC, TB, Ts, Tu, Tt, Tl, Tk, Tj, Tw, Ty, Tx, Tf, Th;
		    V Tg, TA, TJ, TW, TX, T1h, T1i, TM, TN, Te, Tn, T28, T29, T2b, T2c;
		    V T14, T17, T1b, T1e;
		    T2 = LD(&(Rp[0]), ms, &(Rp[0]));
		    TP = LD(&(Rp[WS(rs, 5)]), ms, &(Rp[WS(rs, 1)]));
		    T3 = LD(&(Rm[WS(rs, 9)]), -ms, &(Rm[WS(rs, 1)]));
		    T4 = VCONJ(T3);
		    TQ = LD(&(Rm[WS(rs, 4)]), -ms, &(Rm[0]));
		    TR = VCONJ(TQ);
		    TH = LD(&(Rp[WS(rs, 7)]), ms, &(Rp[WS(rs, 1)]));
		    TF = LD(&(Rm[WS(rs, 2)]), -ms, &(Rm[0]));
		    TG = VCONJ(TF);
		    TI = VSUB(TG, TH);
		    T1d = VADD(TG, TH);
		    T6 = LD(&(Rp[WS(rs, 4)]), ms, &(Rp[0]));
		    T7 = LD(&(Rm[WS(rs, 5)]), -ms, &(Rm[WS(rs, 1)]));
		    T8 = VCONJ(T7);
		    T9 = VSUB(T6, T8);
		    T12 = VADD(T6, T8);
		    Tc = LD(&(Rp[WS(rs, 6)]), ms, &(Rp[0]));
		    Ta = LD(&(Rm[WS(rs, 3)]), -ms, &(Rm[WS(rs, 1)]));
		    Tb = VCONJ(Ta);
		    Td = VSUB(Tb, Tc);
		    T15 = VADD(Tb, Tc);
		    TD = LD(&(Rp[WS(rs, 3)]), ms, &(Rp[WS(rs, 1)]));
		    TB = LD(&(Rm[WS(rs, 6)]), -ms, &(Rm[0]));
		    TC = VCONJ(TB);
		    TE = VSUB(TC, TD);
		    T1a = VADD(TC, TD);
		    Ts = LD(&(Rp[WS(rs, 9)]), ms, &(Rp[WS(rs, 1)]));
		    Tt = LD(&(Rm[0]), -ms, &(Rm[0]));
		    Tu = VCONJ(Tt);
		    Tv = VSUB(Ts, Tu);
		    T13 = VADD(Ts, Tu);
		    Tl = LD(&(Rp[WS(rs, 2)]), ms, &(Rp[0]));
		    Tj = LD(&(Rm[WS(rs, 7)]), -ms, &(Rm[WS(rs, 1)]));
		    Tk = VCONJ(Tj);
		    Tm = VSUB(Tk, Tl);
		    T1c = VADD(Tk, Tl);
		    Tw = LD(&(Rp[WS(rs, 1)]), ms, &(Rp[WS(rs, 1)]));
		    Tx = LD(&(Rm[WS(rs, 8)]), -ms, &(Rm[0]));
		    Ty = VCONJ(Tx);
		    Tz = VSUB(Tw, Ty);
		    T16 = VADD(Tw, Ty);
		    Tf = LD(&(Rp[WS(rs, 8)]), ms, &(Rp[0]));
		    Tg = LD(&(Rm[WS(rs, 1)]), -ms, &(Rm[WS(rs, 1)]));
		    Th = VCONJ(Tg);
		    Ti = VSUB(Tf, Th);
		    T19 = VADD(Tf, Th);
		    TA = VSUB(Tv, Tz);
		    TJ = VSUB(TE, TI);
		    TK = VFNMS(LDK(KP951056516), TJ, VMUL(LDK(KP587785252), TA));
		    T1v = VFMA(LDK(KP951056516), TA, VMUL(LDK(KP587785252), TJ));
		    TW = VSUB(T9, Td);
		    TX = VSUB(Ti, Tm);
		    TY = VFNMS(LDK(KP951056516), TX, VMUL(LDK(KP587785252), TW));
		    T1x = VFMA(LDK(KP951056516), TW, VMUL(LDK(KP587785252), TX));
		    T1h = VADD(T2, T4);
		    T1i = VADD(TP, TR);
		    T1j = VSUB(T1h, T1i);
		    T2f = VADD(T1h, T1i);
		    TS = VSUB(TP, TR);
		    TM = VADD(Tv, Tz);
		    TN = VADD(TE, TI);
		    TT = VADD(TM, TN);
		    TO = VMUL(LDK(KP559016994), VSUB(TM, TN));
		    TU = VFNMS(LDK(KP250000000), TT, TS);
		    T5 = VSUB(T2, T4);
		    Te = VADD(T9, Td);
		    Tn = VADD(Ti, Tm);
		    To = VADD(Te, Tn);
		    Tp = VFNMS(LDK(KP250000000), To, T5);
		    Tq = VMUL(LDK(KP559016994), VSUB(Te, Tn));
		    T28 = VADD(T12, T13);
		    T29 = VADD(T15, T16);
		    T2a = VADD(T28, T29);
		    T2b = VADD(T19, T1a);
		    T2c = VADD(T1c, T1d);
		    T2d = VADD(T2b, T2c);
		    T2g = VADD(T2a, T2d);
		    T2k = VSUB(T2b, T2c);
		    T2j = VSUB(T28, T29);
		    T14 = VSUB(T12, T13);
		    T17 = VSUB(T15, T16);
		    T1k = VADD(T14, T17);
		    T1b = VSUB(T19, T1a);
		    T1e = VSUB(T1c, T1d);
		    T1l = VADD(T1b, T1e);
		    T18 = VSUB(T14, T17);
		    T1m = VADD(T1k, T1l);
		    T1f = VSUB(T1b, T1e);
	       }
	       {
		    V T2L, T22, T1S, T26, T2m, T2G, T2s, T2A, T1q, T1U, T1C, T1M, T10, T2E, T1I;
		    V T2q, T1A, T2K, T20, T2w, T21, T1Q, T1R, T1P, T25, T1r, T1s, T2C, T2N, T1N;
		    V T2H, T2I, T2M, T1E, T1D, T1O, T1V, T2n, T2B, T24, T2o, T2t, T2u, T23, T1W;
		    T2L = VADD(T2f, T2g);
		    T21 = LDW(&(W[TWVL * 18]));
		    T22 = VZMUL(T21, VADD(T1j, T1m));
		    T1Q = VADD(T5, To);
		    T1R = VBYI(VADD(TS, TT));
		    T1P = LDW(&(W[TWVL * 28]));
		    T1S = VZMULI(T1P, VSUB(T1Q, T1R));
		    T25 = LDW(&(W[TWVL * 8]));
		    T26 = VZMULI(T25, VADD(T1Q, T1R));
		    {
			 V T2l, T2z, T2i, T2y, T2e, T2h, T27, T2F, T2r, T2x, T1g, T1K, T1p, T1L, T1n;
			 V T1o, T11, T1T, T1B, T1J, TL, T1G, TZ, T1H, Tr, TV, T1, T2D, T1F, T2p;
			 V T1w, T1Y, T1z, T1Z, T1u, T1y, T1t, T2J, T1X, T2v;
			 T2l = VBYI(VFMA(LDK(KP951056516), T2j, VMUL(LDK(KP587785252), T2k)));
			 T2z = VBYI(VFNMS(LDK(KP951056516), T2k, VMUL(LDK(KP587785252), T2j)));
			 T2e = VMUL(LDK(KP559016994), VSUB(T2a, T2d));
			 T2h = VFNMS(LDK(KP250000000), T2g, T2f);
			 T2i = VADD(T2e, T2h);
			 T2y = VSUB(T2h, T2e);
			 T27 = LDW(&(W[TWVL * 6]));
			 T2m = VZMUL(T27, VSUB(T2i, T2l));
			 T2F = LDW(&(W[TWVL * 22]));
			 T2G = VZMUL(T2F, VADD(T2z, T2y));
			 T2r = LDW(&(W[TWVL * 30]));
			 T2s = VZMUL(T2r, VADD(T2l, T2i));
			 T2x = LDW(&(W[TWVL * 14]));
			 T2A = VZMUL(T2x, VSUB(T2y, T2z));
			 T1g = VBYI(VFNMS(LDK(KP951056516), T1f, VMUL(LDK(KP587785252), T18)));
			 T1K = VBYI(VFMA(LDK(KP951056516), T18, VMUL(LDK(KP587785252), T1f)));
			 T1n = VFNMS(LDK(KP250000000), T1m, T1j);
			 T1o = VMUL(LDK(KP559016994), VSUB(T1k, T1l));
			 T1p = VSUB(T1n, T1o);
			 T1L = VADD(T1o, T1n);
			 T11 = LDW(&(W[TWVL * 2]));
			 T1q = VZMUL(T11, VADD(T1g, T1p));
			 T1T = LDW(&(W[TWVL * 26]));
			 T1U = VZMUL(T1T, VSUB(T1L, T1K));
			 T1B = LDW(&(W[TWVL * 34]));
			 T1C = VZMUL(T1B, VSUB(T1p, T1g));
			 T1J = LDW(&(W[TWVL * 10]));
			 T1M = VZMUL(T1J, VADD(T1K, T1L));
			 Tr = VSUB(Tp, Tq);
			 TL = VSUB(Tr, TK);
			 T1G = VADD(Tr, TK);
			 TV = VSUB(TO, TU);
			 TZ = VBYI(VSUB(TV, TY));
			 T1H = VBYI(VADD(TY, TV));
			 T1 = LDW(&(W[TWVL * 4]));
			 T10 = VZMULI(T1, VADD(TL, TZ));
			 T2D = LDW(&(W[TWVL * 24]));
			 T2E = VZMULI(T2D, VSUB(T1G, T1H));
			 T1F = LDW(&(W[TWVL * 12]));
			 T1I = VZMULI(T1F, VADD(T1G, T1H));
			 T2p = LDW(&(W[TWVL * 32]));
			 T2q = VZMULI(T2p, VSUB(TL, TZ));
			 T1u = VADD(Tq, Tp);
			 T1w = VSUB(T1u, T1v);
			 T1Y = VADD(T1u, T1v);
			 T1y = VADD(TO, TU);
			 T1z = VBYI(VADD(T1x, T1y));
			 T1Z = VBYI(VSUB(T1y, T1x));
			 T1t = LDW(&(W[TWVL * 36]));
			 T1A = VZMULI(T1t, VSUB(T1w, T1z));
			 T2J = LDW(&(W[0]));
			 T2K = VZMULI(T2J, VADD(T1w, T1z));
			 T1X = LDW(&(W[TWVL * 20]));
			 T20 = VZMULI(T1X, VSUB(T1Y, T1Z));
			 T2v = LDW(&(W[TWVL * 16]));
			 T2w = VZMULI(T2v, VADD(T1Y, T1Z));
		    }
		    T1r = VADD(T10, T1q);
		    ST(&(Rp[WS(rs, 1)]), T1r, ms, &(Rp[WS(rs, 1)]));
		    T1s = VCONJ(VSUB(T1q, T10));
		    ST(&(Rm[WS(rs, 1)]), T1s, -ms, &(Rm[WS(rs, 1)]));
		    T2C = VCONJ(VSUB(T2A, T2w));
		    ST(&(Rm[WS(rs, 4)]), T2C, -ms, &(Rm[0]));
		    T2N = VCONJ(VSUB(T2L, T2K));
		    ST(&(Rm[0]), T2N, -ms, &(Rm[0]));
		    T1N = VADD(T1I, T1M);
		    ST(&(Rp[WS(rs, 3)]), T1N, ms, &(Rp[WS(rs, 1)]));
		    T2H = VADD(T2E, T2G);
		    ST(&(Rp[WS(rs, 6)]), T2H, ms, &(Rp[0]));
		    T2I = VCONJ(VSUB(T2G, T2E));
		    ST(&(Rm[WS(rs, 6)]), T2I, -ms, &(Rm[0]));
		    T2M = VADD(T2K, T2L);
		    ST(&(Rp[0]), T2M, ms, &(Rp[0]));
		    T1E = VCONJ(VSUB(T1C, T1A));
		    ST(&(Rm[WS(rs, 9)]), T1E, -ms, &(Rm[WS(rs, 1)]));
		    T1D = VADD(T1A, T1C);
		    ST(&(Rp[WS(rs, 9)]), T1D, ms, &(Rp[WS(rs, 1)]));
		    T1O = VCONJ(VSUB(T1M, T1I));
		    ST(&(Rm[WS(rs, 3)]), T1O, -ms, &(Rm[WS(rs, 1)]));
		    T1V = VADD(T1S, T1U);
		    ST(&(Rp[WS(rs, 7)]), T1V, ms, &(Rp[WS(rs, 1)]));
		    T2n = VADD(T26, T2m);
		    ST(&(Rp[WS(rs, 2)]), T2n, ms, &(Rp[0]));
		    T2B = VADD(T2w, T2A);
		    ST(&(Rp[WS(rs, 4)]), T2B, ms, &(Rp[0]));
		    T24 = VCONJ(VSUB(T22, T20));
		    ST(&(Rm[WS(rs, 5)]), T24, -ms, &(Rm[WS(rs, 1)]));
		    T2o = VCONJ(VSUB(T2m, T26));
		    ST(&(Rm[WS(rs, 2)]), T2o, -ms, &(Rm[0]));
		    T2t = VADD(T2q, T2s);
		    ST(&(Rp[WS(rs, 8)]), T2t, ms, &(Rp[0]));
		    T2u = VCONJ(VSUB(T2s, T2q));
		    ST(&(Rm[WS(rs, 8)]), T2u, -ms, &(Rm[0]));
		    T23 = VADD(T20, T22);
		    ST(&(Rp[WS(rs, 5)]), T23, ms, &(Rp[WS(rs, 1)]));
		    T1W = VCONJ(VSUB(T1U, T1S));
		    ST(&(Rm[WS(rs, 7)]), T1W, -ms, &(Rm[WS(rs, 1)]));
	       }
	  }
     }
     VLEAVE();
}

static const tw_instr twinstr[] = {
     VTW(1, 1),
     VTW(1, 2),
     VTW(1, 3),
     VTW(1, 4),
     VTW(1, 5),
     VTW(1, 6),
     VTW(1, 7),
     VTW(1, 8),
     VTW(1, 9),
     VTW(1, 10),
     VTW(1, 11),
     VTW(1, 12),
     VTW(1, 13),
     VTW(1, 14),
     VTW(1, 15),
     VTW(1, 16),
     VTW(1, 17),
     VTW(1, 18),
     VTW(1, 19),
     {TW_NEXT, VL, 0}
};

static const hc2c_desc desc = { 20, XSIMD_STRING("hc2cbdftv_20"), twinstr, &GENUS, {131, 50, 12, 0} };

void XSIMD(codelet_hc2cbdftv_20) (planner *p) {
     X(khc2c_register) (p, hc2cbdftv_20, &desc, HC2C_VIA_DFT);
}
#endif				/* HAVE_FMA */
