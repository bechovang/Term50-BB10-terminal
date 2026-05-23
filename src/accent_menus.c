#include <unicode/utf.h>
#include "types.h"

const int accent_row_lens[] = {  \
	/* a */ 8, /* b */ 1, /* c */ 3, /* d */ 2, /* e */ 5, /* f */ 1, \
	/* g */ 1, /* h */ 1, /* i */ 5, /* j */ 1, /* k */ 1, /* l */ 1, \
	/* m */ 1, /* n */ 2, /* o */ 8, /* p */ 2, /* q */ 1, /* r */ 1, \
	/* s */ 3, /* t */ 3, /* u */ 5, /* v */ 1, /* w */ 1, /* x */ 1, \
	/* y */ 3, /* z */ 1 };

keymap_t lc_a_entry[] = {{'q', NULL, "A"}, {'w', NULL, "\u00E0"}, {'e', NULL, "\u00E1"}, {'r', NULL, "\u00E2"}, {'t', NULL, "\u00E3"}, {'y', NULL, "\u00E4"}, {'u', NULL, "\u00E5"}, {'i', NULL, "\u00E6"}};
keymap_t lc_b_entry[] = {{'q', NULL, "B"}};
keymap_t lc_c_entry[] = {{'q', NULL, "C"}, {'w', NULL, "\u00E7"}, {'e', NULL, "\u00A9"}};
keymap_t lc_d_entry[] = {{'q', NULL, "D"}, {'w', NULL, "\u00F0"}};
keymap_t lc_e_entry[] = {{'q', NULL, "E"}, {'w', NULL, "\u00E9"}, {'e', NULL, "\u00E8"}, {'r', NULL, "\u00EA"}, {'t', NULL, "\u00EB"}};
keymap_t lc_f_entry[] = {{'q', NULL, "F"}};
keymap_t lc_g_entry[] = {{'q', NULL, "G"}};
keymap_t lc_h_entry[] = {{'q', NULL, "H"}};
keymap_t lc_i_entry[] = {{'q', NULL, "I"}, {'w', NULL, "\u00EC"}, {'e', NULL, "\u00ED"}, {'r', NULL, "\u00EE"}, {'t', NULL, "\u00EF"}};
keymap_t lc_j_entry[] = {{'q', NULL, "J"}};
keymap_t lc_k_entry[] = {{'q', NULL, "K"}};
keymap_t lc_l_entry[] = {{'q', NULL, "L"}};
keymap_t lc_m_entry[] = {{'q', NULL, "M"}};
keymap_t lc_n_entry[] = {{'q', NULL, "N"}, {'w', NULL, "\u00F1"}};
keymap_t lc_o_entry[] = {{'q', NULL, "O"}, {'w', NULL, "\u00F2"}, {'e', NULL, "\u00F3"}, {'r', NULL, "\u00F4"}, {'t', NULL, "\u00F5"}, {'y', NULL, "\u00F6"}, {'u', NULL, "\u0153"}, {'i', NULL, "\u00F8"}};
keymap_t lc_p_entry[] = {{'q', NULL, "P"}, {'w', NULL, "\u00B6"}};
keymap_t lc_q_entry[] = {{'q', NULL, "Q"}};
keymap_t lc_r_entry[] = {{'q', NULL, "R"}};
keymap_t lc_s_entry[] = {{'q', NULL, "S"}, {'w', NULL, "\u00A7"}, {'e', NULL, "\u00DF"}};
keymap_t lc_t_entry[] = {{'q', NULL, "T"}, {'w', NULL, "\u00DE"}};
keymap_t lc_u_entry[] = {{'q', NULL, "U"}, {'w', NULL, "\u00F9"}, {'e', NULL, "\u00FA"}, {'r', NULL, "\u00FB"}, {'t', NULL, "\u00FC"}};
keymap_t lc_v_entry[] = {{'q', NULL, "V"}};
keymap_t lc_w_entry[] = {{'q', NULL, "W"}};
keymap_t lc_x_entry[] = {{'q', NULL, "X"}};
keymap_t lc_y_entry[] = {{'q', NULL, "Y"}, {'w', NULL, "\u00FD"}, {'e', NULL, "\u00FF"}};
keymap_t lc_z_entry[] = {{'q', NULL, "Z"}};

const keymap_t* lowercase_accent_entries[26] = {
	lc_a_entry, lc_b_entry, lc_c_entry, lc_d_entry, lc_e_entry, lc_f_entry, lc_g_entry,
	lc_h_entry, lc_i_entry, lc_j_entry, lc_k_entry, lc_l_entry, lc_m_entry, lc_n_entry,
	lc_o_entry, lc_p_entry, lc_q_entry, lc_r_entry, lc_s_entry, lc_t_entry, lc_u_entry,
	lc_v_entry, lc_w_entry, lc_x_entry, lc_y_entry, lc_z_entry
};

keymap_t uc_a_entry[] = {{'q', NULL, "A"}, {'w', NULL, "\u00C0"}, {'e', NULL, "\u00C1"}, {'r', NULL, "\u00C2"}, {'t', NULL, "\u00C3"}, {'y', NULL, "\u00C4"}, {'u', NULL, "\u00C5"}, {'i', NULL, "\u00C6"}};
keymap_t uc_b_entry[] = {{'q', NULL, "B"}};
keymap_t uc_c_entry[] = {{'q', NULL, "C"}, {'w', NULL, "\u00C7"}, {'e', NULL, "\u00A9"}};
keymap_t uc_d_entry[] = {{'q', NULL, "D"}, {'w', NULL, "\u00D0"}};
keymap_t uc_e_entry[] = {{'q', NULL, "E"}, {'w', NULL, "\u00C9"}, {'e', NULL, "\u00C8"}, {'r', NULL, "\u00CA"}, {'t', NULL, "\u00CB"}};
keymap_t uc_f_entry[] = {{'q', NULL, "F"}};
keymap_t uc_g_entry[] = {{'q', NULL, "G"}};
keymap_t uc_h_entry[] = {{'q', NULL, "H"}};
keymap_t uc_i_entry[] = {{'q', NULL, "I"}, {'w', NULL, "\u00CC"}, {'e', NULL, "\u00CD"}, {'r', NULL, "\u00CE"}, {'t', NULL, "\u00CF"}};
keymap_t uc_j_entry[] = {{'q', NULL, "J"}};
keymap_t uc_k_entry[] = {{'q', NULL, "K"}};
keymap_t uc_l_entry[] = {{'q', NULL, "L"}};
keymap_t uc_m_entry[] = {{'q', NULL, "M"}};
keymap_t uc_n_entry[] = {{'q', NULL, "N"}, {'w', NULL, "\u00D1"}};
keymap_t uc_o_entry[] = {{'q', NULL, "O"}, {'w', NULL, "\u00D2"}, {'e', NULL, "\u00D3"}, {'r', NULL, "\u00D4"}, {'t', NULL, "\u00D5"}, {'y', NULL, "\u00D6"}, {'u', NULL, "\u0152"}, {'i', NULL, "\u00D8"}};
keymap_t uc_p_entry[] = {{'q', NULL, "P"}, {'w', NULL, "\u00B6"}};
keymap_t uc_q_entry[] = {{'q', NULL, "Q"}};
keymap_t uc_r_entry[] = {{'q', NULL, "R"}};
keymap_t uc_s_entry[] = {{'q', NULL, "S"}, {'w', NULL, "\u00A7"}, {'e', NULL, "\u00DF"}};
keymap_t uc_t_entry[] = {{'q', NULL, "T"}, {'w', NULL, "\u00DE"}};
keymap_t uc_u_entry[] = {{'q', NULL, "U"}, {'w', NULL, "\u00D9"}, {'e', NULL, "\u00DA"}, {'r', NULL, "\u00DB"}, {'t', NULL, "\u00DC"}};
keymap_t uc_v_entry[] = {{'q', NULL, "V"}};
keymap_t uc_w_entry[] = {{'q', NULL, "W"}};
keymap_t uc_x_entry[] = {{'q', NULL, "X"}};
keymap_t uc_y_entry[] = {{'q', NULL, "Y"}, {'w', NULL, "\u00DD"}, {'e', NULL, "\u00DF"}};
keymap_t uc_z_entry[] = {{'q', NULL, "Z"}};

const keymap_t* uppercase_accent_entries[26] = {
	uc_a_entry, uc_b_entry, uc_c_entry, uc_d_entry, uc_e_entry, uc_f_entry, uc_g_entry,
	uc_h_entry, uc_i_entry, uc_j_entry, uc_k_entry, uc_l_entry, uc_m_entry, uc_n_entry,
	uc_o_entry, uc_p_entry, uc_q_entry, uc_r_entry, uc_s_entry, uc_t_entry, uc_u_entry,
	uc_v_entry, uc_w_entry, uc_x_entry, uc_y_entry, uc_z_entry
};
