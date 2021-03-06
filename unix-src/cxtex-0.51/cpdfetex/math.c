

#include "types.h"
#include "c-compat.h"
#include "globals.h"

/* module 147 */

/* A |math_node|, which occurs only in horizontal lists, appears before and
 * after mathematical formulas. The |subtype| field is |before| before the
 * formula and |after| after it. There is a |width| field, which represents
 * the amount of surrounding space inserted by \.{\\mathsurround}.
 * 
 * In addition a |math_node| with |subtype>after| and |width=0| will be
 * (ab)used to record a regular |math_node| reinserted after being
 * discarded at a line break or one of the text direction primitives (
 * \.{\\beginL}, \.{\\endL}, \.{\\beginR}, and \.{\\endR} ).
 */
pointer
new_math (scaled w, small_number s) {
  pointer p;			/* the new node */
  p = get_node (small_node_size);
  type (p) = math_node;
  subtype (p) = s;
  width (p) = w;
  return p;
};



/* module 824 */

/* 
 * When \TeX\ reads a formula that is enclosed between \.\$'s, it constructs an
 * {\sl mlist}, which is essentially a tree structure representing that
 * formula. An mlist is a linear sequence of items, but we can regard it as
 * a tree structure because mlists can appear within mlists. For example, many
 * of the entries can be subscripted or superscripted, and such ``scripts''
 * are mlists in their own right.
 * 
 * An entire formula is parsed into such a tree before any of the actual
 * typesetting is done, because the current style of type is usually not
 * known until the formula has been fully scanned. For example, when the
 * formula `\.{\$a+b \\over c+d\$}' is being read, there is no way to tell
 * that `\.{a+b}' will be in script size until `\.{\\over}' has appeared.
 * 
 * During the scanning process, each element of the mlist being built is
 * classified as a relation, a binary operator, an open parenthesis, etc.,
 * or as a construct like `\.{\\sqrt}' that must be built up. This classification
 * appears in the mlist data structure.
 * 
 * After a formula has been fully scanned, the mlist is converted to an hlist
 * so that it can be incorporated into the surrounding text. This conversion is
 * controlled by a recursive procedure that decides all of the appropriate
 * styles by a ``top-down'' process starting at the outermost level and working
 * in towards the subformulas. The formula is ultimately pasted together using
 * combinations of horizontal and vertical boxes, with glue and penalty nodes
 * inserted as necessary.
 * 
 * An mlist is represented internally as a linked list consisting chiefly
 * of ``noads'' (pronounced ``no-adds''), to distinguish them from the somewhat
 * similar ``nodes'' in hlists and vlists. Certain kinds of ordinary nodes are
 * allowed to appear in mlists together with the noads; \TeX\ tells the difference
 * by means of the |type| field, since a noad's |type| is always greater than
 * that of a node. An mlist does not contain character nodes, hlist nodes, vlist
 * nodes, math nodes, ligature nodes,
 * or unset nodes; in particular, each mlist item appears in the
 * variable-size part of |mem|, so the |type| field is always present.
 */



/* module 828 */

/* The global variable |empty_field| is set up for initialization of empty
 * fields in new noads. Similarly, |null_delimiter| is for the initialization
 * of delimiter fields.
 */

two_halves empty_field;
four_quarters null_delimiter;

/* module 829 */

void
math_initialize (void) {
  empty_field.rh = empty;
  empty_field.lhfield = null;
  null_delimiter.b0 = 0;
  null_delimiter.b1 = min_quarterword;
  null_delimiter.b2 = 0;
  null_delimiter.b3 = min_quarterword;
}


/* module 830 */
/*
 * The |new_noad| function creates an |ord_noad| that is completely null.
 */
pointer new_noad (void) {
  pointer p;
  p = get_node (noad_size);
  type (p) = ord_noad;
  subtype (p) = normal;
  mem[nucleus (p)].hh = empty_field;
  mem[subscr (p)].hh = empty_field;
  mem[supscr (p)].hh = empty_field;
  return p;
};


/* module 832 */

/* Math formulas can also contain instructions like \.{\\textstyle} that
 * override \TeX's normal style rules. A |style_node| is inserted into the
 * data structure to record such instructions; it is three words long, so it
 * is considered a node instead of a noad. The |subtype| is either |display_style|
 * or |text_style| or |script_style| or |script_script_style|. The
 * second and third words of a |style_node| are not used, but they are
 * present because a |choice_node| is converted to a |style_node|.
 * 
 * \TeX\ uses even numbers 0, 2, 4, 6 to encode the basic styles
 * |display_style|, \dots, |script_script_style|, and adds~1 to get the
 * ``cramped'' versions of these styles. This gives a numerical order that
 * is backwards from the convention of Appendix~G in {\sl The \TeX book\/};
 * i.e., a smaller style has a larger numerical value.
 */
pointer new_style (small_number s) {
  /* create a style node */
  pointer p; /* the new node */
  p = get_node (style_node_size);
  type (p) = style_node;
  subtype (p) = s;
  width (p) = 0;
  depth (p) = 0;  /* the |width| and |depth| are not used */ 
  return p;
};


/* module 833 */
/*
 * Finally, the \.{\\mathchoice} primitive creates a |choice_node|, which
 * has special subfields |display_mlist|, |text_mlist|, |script_mlist|,
 * and |script_script_mlist| pointing to the mlists for each style.
 */
pointer new_choice (void) { /* create a choice node */
  pointer p; /* the new node */
  p = get_node (style_node_size);
  type (p) = choice_node;
  subtype (p) = 0; /* the |subtype| is not used */
  display_mlist (p) = null;
  text_mlist (p) = null;
  script_mlist (p) = null;
  script_script_mlist (p) = null;
  return p;
};

/* module 835 */
/* 
 * Here are some simple routines used in the display of noads.
 */
void
print_fam_and_char (pointer p) { /* prints family and character */
  print_esc_string ("fam");
  print_int (fam (p));
  print_char (' ');
  print_ASCII (qo (character (p)));
};

void
print_delimiter (pointer p) { /* prints a delimiter as 24-bit hex value */
  int a;			/* accumulator */
  a = small_fam (p) * 256 + qo (small_char (p));
  a = a * 4096 + large_fam (p) * 256 + qo (large_char (p));
  if (a < 0) {
	print_int (a);		/* this should never happen */
  } else {
	print_hex (a);
  };
};

/* module 836 */

/* The next subroutine will descend to another level of recursion when a
 * subsidiary mlist needs to be displayed. The parameter |c| indicates what
 * character is to become part of the recursion history. An empty mlist is
 * distinguished from a field with |math_type(p)=empty|, because these are
 * not equivalent (as explained above).
 */

void 
print_subsidiary_data (pointer p, ASCII_code c) {
  /* display a noad field */
  if (cur_length >= depth_threshold) {
	if (math_type (p) != empty)
	  zprint_string (" []");
  } else {
	append_char (c);		/* include |c| in the recursion history */
	temp_ptr = p;		/* prepare for |show_info| if recursion is needed */
	switch (math_type (p)) {
	case math_char:
	  {
	    print_ln();
	    print_current_string();
	    print_fam_and_char (p);
	  };
	  break;
	case sub_box:
	  show_node_list(info(temp_ptr));
	  break;
	case sub_mlist:	/* recursive call */
	  if (info (p) == null) {
		print_ln();
		print_current_string();
		zprint_string ("{}");
	  } else {
		show_node_list(info(temp_ptr));
	  }
	  break;
	};
	flush_char; /* remove |c| from the recursion history */
  };
};


/* module 838 */
void
print_style (int c) {
  switch (c / 2) {
    case 0:
      print_esc_string ("displaystyle");
      break;
    case 1:			/* |display_style=0| */
      print_esc_string ("textstyle");
      break;
    case 2:			/* |text_style=2| */
      print_esc_string ("scriptstyle");
      break;
    case 3:			/* |script_style=4| */
      print_esc_string ("scriptscriptstyle");
      break;
    default:
      zprint_string ("Unknown style!");
  };
};

/* module 844 */

/* Before an mlist is converted to an hlist, \TeX\ makes sure that
 * the fonts in family~2 have enough parameters to be math-symbol
 * fonts, and that the fonts in family~3 have enough parameters to be
 * math-extension fonts. The math-symbol parameters are referred to by using the
 * following macros, which take a size code as their parameter; for example,
 * |num1(cur_size)| gives the value of the |num1| parameter for the current size.
 */
#define mathsy( a,b )   font_info [a + param_base[fam_fnt (2 + b)]].sc
#define math_x_height(a)  mathsy (5,a)
#define num1(a)  mathsy (8,a)
#define num2(a)  mathsy (9,a)
#define num3(a)  mathsy (10,a)
#define denom1(a)  mathsy (11,a)
#define denom2(a) mathsy (12,a)
#define sup1(a)  mathsy (13,a)
#define sup2(a)  mathsy (14,a)
#define sup3(a)  mathsy (15,a)
#define sub1(a)  mathsy (16,a)
#define sub2(a)  mathsy (17,a)
#define sup_drop(a)  mathsy (18,a)
#define sub_drop(a)  mathsy (19,a)
#define delim1(a)  mathsy (20,a)
#define delim2(a)  mathsy (21,a)
#define axis_height(a)  mathsy (22,a)

/* module 845 */
/* The math-extension parameters have similar macros, but the size code is
 * omitted (since it is always |cur_size| when we refer to such parameters).
 */
#define mathex( arg )  font_info [ arg  +  param_base [ fam_fnt (3  +  cur_size )]]. sc
#define default_rule_thickness  mathex (8 )
#define big_op_spacing1  mathex (9 )
#define big_op_spacing2  mathex (10 )
#define big_op_spacing3  mathex (11 )
#define big_op_spacing4  mathex (12 )
#define big_op_spacing5  mathex (13 )

/* module 846 */

/* We also need to compute the change in style between mlists and their
 * subsidiaries. The following macros define the subsidiary style for
 * an overlined nucleus (|cramped_style|), for a subscript or a superscript
 * (|sub_style| or |sup_style|), or for a numerator or denominator (|num_style|
 * or |denom_style|).
 */
#define cramped_style( arg ) 2  * ( arg   / 2 ) +  cramped
#define sub_style( arg ) 2  * ( arg   / 4 ) +  script_style  +  cramped
#define sup_style( arg ) 2  * ( arg   / 4 ) +  script_style  + ( arg   % 2 )
#define num_style( arg )  arg  + 2  - 2  * ( arg   / 6 )
#define denom_style( arg ) 2  * ( arg   / 2 ) +  cramped  + 2  - 2  * ( arg   / 6 )



/* module 847 */

/* When the style changes, the following piece of program computes associated
 * information:
 */
#define set_cur_size_and_mu	    { if (cur_style < script_style) {\
			cur_size = text_size;\
		} else	cur_size = 16 * ((cur_style - text_style) / 2);\
	      cur_mu = x_over_n (math_quad (cur_size), 18); }\




/* module 848 */

/* Here is a function that returns a pointer to a rule node having a given
 * thickness |t|. The rule will extend horizontally to the boundary of the vlist
 * that eventually contains it.
 */
pointer
fraction_rule (scaled t) { /* construct the bar for a fraction */
  pointer p;  /* the new node */ 
  p = new_rule();
  height (p) = t;
  depth (p) = 0;
  return p;
};


/* module 849 */

/* The |overbar| function returns a pointer to a vlist box that consists of
 * a given box |b|, above which has been placed a kern of height |k| under a
 * fraction rule of thickness |t| under additional space of height |t|.
 */
pointer 
overbar (pointer b, scaled k, scaled t) {
  pointer p, q;  /* nodes being constructed */ 
  p = new_kern (k);
  link (p) = b;
  q = fraction_rule (t);
  link (q) = p;
  p = new_kern (t);
  link (p) = q;
  return VPACK (p, 0, additional);
};

/* module 850 */

/* The |var_delimiter| function, which finds or constructs a sufficiently
 * large delimiter, is the most interesting of the auxiliary functions that
 * currently concern us. Given a pointer |d| to a delimiter field in some noad,
 * together with a size code |s| and a vertical distance |v|, this function
 * returns a pointer to a box that contains the smallest variant of |d| whose
 * height plus depth is |v| or more. (And if no variant is large enough, it
 * returns the largest available variant.) In particular, this routine will
 * construct arbitrarily large delimiters from extensible components, if
 * |d| leads to such characters.
 * 
 * The value returned is a box whose |shift_amount| has been set so that
 * the box is vertically centered with respect to the axis in the given size.
 * If a built-up symbol is returned, the height of the box before shifting
 * will be the height of its topmost component.
 */

/* module 853 */
/*
 * Here is a subroutine that creates a new box, whose list contains a
 * single character, and whose width includes the italic correction for
 * that character. The height or depth of the box will be negative, if
 * the height or depth of the character is negative; thus, this routine
 * may deliver a slightly different result than |hpack| would produce.
 */

pointer 
char_box (internal_font_number f, quarterword c) {
  four_quarters q;
  eight_bits hd; /* |height_depth| byte */ 
  pointer b, p; /* the new box and its character node */ 
  q = char_info (f, c);
  hd = height_depth (q);
  b = new_null_box();
  width (b) = char_width (f, q) + char_italic (f, q);
  height (b) = char_height (f, hd);
  depth (b) = char_depth (f, hd);
  p = get_avail();
  character (p) = c;
  font (p) = f;
  list_ptr (b) = p;
  return b;
};

/* module 855 */
/*
 * When we build an extensible character, it's handy to have the
 * following subroutine, which puts a given character on top
 * of the characters already in box |b|:
 */

void 
stack_into_box (pointer b, internal_font_number f, quarterword c) {
  pointer p; /* new node placed into |b| */ 
  p = char_box (f, c);
  link (p) = list_ptr (b);
  list_ptr (b) = p;
  height (b) = height (p);
};


/* module 856 */
/*
 * Another handy subroutine computes the height plus depth of
 * a given character:
 */

scaled 
height_plus_depth (internal_font_number f, quarterword c) {
  four_quarters q;
  eight_bits hd; /* |height_depth| byte */ 
  q = char_info (f, c);
  hd = height_depth (q);
  return char_height (f, hd) + char_depth (f, hd);
};


pointer 
var_delimiter (pointer d, small_number s, scaled v) {
  pointer b;  /* the box that will be constructed */ 
  internal_font_number f, g; /* best-so-far and tentative font codes */ 
  quarterword c, x, y; /* best-so-far and tentative character codes */ 
  int m, n; /* the number of extensible pieces */ 
  scaled u; /* height-plus-depth of a tentative character */ 
  scaled w; /* largest height-plus-depth so far */ 
  four_quarters q; /* character info */ 
  eight_bits hd; /* height-depth byte */ 
  four_quarters r; /* extensible pieces */ 
  small_number z; /* runs through font family members */ 
  boolean large_attempt; /* are we trying the ``large'' variant? */
  c=0; /*TH -Wall*/ 
  f = null_font;
  w = 0;
  large_attempt = false;
  z = small_fam (d);
  x = small_char (d);
  loop {
	/* begin expansion of Look at the variants of |(z,x)|; set |f| and |c| whenever 
	   a better character is found; |goto found| as soon as a large enough variant 
	   is encountered */
	/* module 851 */
	/* The search process is complicated slightly by the facts that some of the
	 * characters might not be present in some of the fonts, and they might not
	 * be probed in increasing order of height.
	 */
	if ((z != 0) || (x != min_quarterword)) {
	  z = z + s + 16;
	  do {
		z = z - 16;
		g = fam_fnt (z);
		if (g != null_font)
		  /* begin expansion of Look at the list of characters starting with |x| 
			 in font |g|; set |f| and |c| whenever a better character is found; 
			 |goto found| as soon as a large enough variant is encountered */
		  /* module 852 */
		  {
			y = x;
			if ((qo (y) >= font_bc[g]) && (qo (y) <= font_ec[g])) {
			CONTINUE:
			  q = orig_char_info (g, y);
			  if (char_exists (q)) {
				if (char_tag (q) == ext_tag) {
				  f = g;
				  c = y;
				  goto FOUND;
				};
				hd = height_depth (q);
				u = char_height (g, hd) + char_depth (g, hd);
				if (u > w) {
				  f = g;
				  c = y;
				  w = u;
				  if (u >= v)
					goto FOUND;
				};
				if (char_tag (q) == list_tag) {
				  y = rem_byte (q);
				  goto CONTINUE;
				};
			  };
			};
		  }
		/* end expansion of Look at the list of ...*/
		;
	  } while (!(z < 16));
	}
	/* end expansion of Look at the variants of |(z,x)|... */
	;
	if (large_attempt)
	  goto FOUND;	/* there were none large enough */ 
	large_attempt = true;
	z = large_fam (d);
	x = large_char (d);
  };
 FOUND:
  if (f != null_font) {
	/* begin expansion of Make variable |b| point to a box for |(f,c)| */
	/* module 854 */
	/* When the following code is executed, |char_tag(q)| will be equal to
	 * |ext_tag| if and only if a built-up symbol is supposed to be returned.
	 */
	if (char_tag (q) == ext_tag)
	  /* begin expansion of Construct an extensible character in a new box |b|, 
		 using recipe |rem_byte(q)| and font |f| */
	  /* module 857 */
	  {
		b = new_null_box();
		type (b) = vlist_node;
		r = font_info[exten_base[f] + rem_byte (q)].qqqq;
		/* begin expansion of Compute the minimum suitable height, |w|, and the 
		   corresponding number of extension steps, |n|; also set |width(b)| */
		/* module 858 */
		/* The width of an extensible character is the width of the repeatable
		 * module. If this module does not have positive height plus depth,
		 * we don't use any copies of it, otherwise we use as few as possible
		 * (in groups of two if there is a middle part).
		 */
		c = ext_rep (r);
		u = height_plus_depth (f, c);
		w = 0;
		q = char_info (f, c);
		width (b) = char_width (f, q) + char_italic (f, q);
		c = ext_bot (r);
		if (c != min_quarterword)
		  w = w + height_plus_depth (f, c);
		c = ext_mid (r);
		if (c != min_quarterword)
		  w = w + height_plus_depth (f, c);
		c = ext_top (r);
		if (c != min_quarterword)
		  w = w + height_plus_depth (f, c);
		n = 0;
		if (u > 0)
		  while (w < v) {
			w = w + u;
			incr (n);
			if (ext_mid (r) != min_quarterword)
			  w = w + u;
		  }
		/* end expansion of Compute the minimum suitable height ....*/
		;
		c = ext_bot (r);
		if (c != min_quarterword)
		  stack_into_box (b, f, c);
		c = ext_rep (r);
		for (m = 1; m <= n; m++)
		  stack_into_box (b, f, c);
		c = ext_mid (r);
		if (c != min_quarterword) {
		  stack_into_box (b, f, c);
		  c = ext_rep (r);
		  for (m = 1; m <= n; m++)
			stack_into_box (b, f, c);
		};
		c = ext_top (r);
		if (c != min_quarterword)
		  stack_into_box (b, f, c);
		depth (b) = w - height (b);
	  }
	/* end expansion of Construct an extensible character in a new.. */
	else {
	  b = char_box (f, c); 
	}
	/* end expansion of Make variable |b| point to a box for |(f,c)| */
  } else {
	b = new_null_box();
	width (b) = null_delimiter_space; /* use this width if no delimiter was found */ 
  };
  shift_amount (b) = half (height (b) - depth (b)) - axis_height (s);
  return b;
};


/* module 859 */

/* The next subroutine is much simpler; it is used for numerators and
 * denominators of fractions as well as for displayed operators and
 * their limits above and below. It takes a given box~|b| and
 * changes it so that the new box is centered in a box of width~|w|.
 * The centering is done by putting \.{\\hss} glue at the left and right
 * of the list inside |b|, then packaging the new box; thus, the
 * actual box might not really be centered, if it already contains
 * infinite glue.
 * 
 * The given box might contain a single character whose italic correction
 * has been added to the width of the box; in this case a compensating
 * kern is inserted.
 */
pointer 
rebox (pointer b, scaled w) {
  pointer p; /* temporary register for list manipulation */
  internal_font_number f; /* font in a one-character box */ 
  scaled v; /* width of a character without italic correction */
  if ((width (b) != w) && (list_ptr (b) != null)) {
	if (type (b) == vlist_node)
	  b = hpack (b, 0, additional);
	p = list_ptr (b);
	if ((is_char_node (p)) && (link (p) == null)) {
	  f = font (p);
	  v = char_width (f, char_info (f, character (p)));
	  if (v != width (b))
		link (p) = new_kern (width (b) - v);
	};
	free_node (b, box_node_size);
	b = new_glue (ss_glue);
	link (b) = p;
	while (link (p) != null)
	  p = link (p);
	link (p) = new_glue (ss_glue);
	return hpack (b, w, exactly);
  } else {
	width (b) = w;
	return b;
  };
};


/* module 860 */

/* Here is a subroutine that creates a new glue specification from another
 * one that is expressed in `\.{mu}', given the value of the math unit.
 */
#define mu_mult( arg )  NX_PLUS_Y ( n , arg , xn_over_d ( arg , f ,65536 ))

pointer 
math_glue (pointer g, scaled m) {
  pointer p; /* the new glue specification */ 
  int n; /* integer part of |m| */ 
  scaled f; /* fraction part of |m| */
  n = x_over_n (m, 65536);
  f = tex_remainder;
  if (f < 0) {
	decr (n);
	f = f + 65536;
  };
  p = get_node (glue_spec_size);
  width (p) = mu_mult (width (g)); /* convert \.{mu} to \.{pt} */ 
  stretch_order (p) = stretch_order (g);
  if (stretch_order (p) == normal) {
	stretch (p) = mu_mult (stretch (g));
  } else {
	stretch (p) = stretch (g);
  }
  shrink_order (p) = shrink_order (g);
  if (shrink_order (p) == normal) {
	shrink (p) = mu_mult (shrink (g));
  } else {
	shrink (p) = shrink (g);
  }
  return p;
};


/* module 861 */

/* The |math_kern| subroutine removes |mu_glue| from a kern node, given
 * the value of the math unit.
 */
void 
math_kern (pointer p, scaled m) {
  int n; /* integer part of |m| */ 
  scaled f; /* fraction part of |m| */ 
  if (subtype (p) == mu_glue) {
	n = x_over_n (m, 65536);
	f = tex_remainder;
	if (f < 0) {
	  decr (n);
	  f = f + 65536;
	};
	width (p) = mu_mult (width (p));
	subtype (p) = explicit;
  };
};


/* module 862 */

/* Sometimes it is necessary to destroy an mlist. The following
 * subroutine empties the current list, assuming that |abs(mode)=mmode|.
 */
void 
flush_math (void) {
  flush_node_list (link (head));
  flush_node_list (incompleat_noad);
  link (head) = null;
  tail = head;
  incompleat_noad = null;
};


/* module 863 */

/* 
 * \TeX's most important routine for dealing with formulas is called
 * |mlist_to_hlist|. After a formula has been scanned and represented as an
 * mlist, this routine converts it to an hlist that can be placed into a box
 * or incorporated into the text of a paragraph. There are three implicit
 * parameters, passed in global variables: |cur_mlist| points to the first
 * node or noad in the given mlist (and it might be |null|); |cur_style| is a
 * style code; and |mlist_penalties| is |true| if penalty nodes for potential
 * line breaks are to be inserted into the resulting hlist. After
 * |mlist_to_hlist| has acted, |link(temp_head)| points to the translated hlist.
 * 
 * Since mlists can be inside mlists, the procedure is recursive. And since this
 * is not part of \TeX's inner loop, the program has been written in a manner
 * that stresses compactness over efficiency.
 */

pointer cur_mlist;/* beginning of mlist to be translated */
small_number cur_style;/* style code at current place in the list */
small_number cur_size;/* size code corresponding to |cur_style| */
scaled cur_mu;/* the math unit width corresponding to |cur_size| */
boolean mlist_penalties;/* should |mlist_to_hlist| insert penalties? */


/* module 864 */

/* The recursion in |mlist_to_hlist| is due primarily to a subroutine
 * called |clean_box| that puts a given noad field into a box using a given
 * math style; |mlist_to_hlist| can call |clean_box|, which can call
 * |mlist_to_hlist|.
 * 
 * The box returned by |clean_box| is ``clean'' in the
 * sense that its |shift_amount| is zero.
 */
pointer 
clean_box (pointer p, small_number s) {
  pointer q; /* beginning of a list to be boxed */ 
  small_number save_style; /* |cur_style| to be restored */ 
  pointer x; /* box to be returned */ 
  pointer r; /* temporary pointer */ 
  switch (math_type (p)) {
  case math_char:
	cur_mlist = new_noad();
	mem[nucleus (cur_mlist)] = mem[p];
	break;
  case sub_box:
	q = info (p);
	goto FOUND;
  case sub_mlist:
	cur_mlist = info (p);
	break;
  default:
	q = new_null_box();
	goto FOUND;
  };
  save_style = cur_style;
  cur_style = s;
  mlist_penalties = false;
  mlist_to_hlist();
  q = link (temp_head); /* recursive call */ 
  cur_style = save_style; /* restore the style */
  /* Set up the values of |cur_size| and |cur_mu|, based on |cur_style| */
  set_cur_size_and_mu;
 FOUND:
  if (is_char_node (q) || (q == null)) {
	x = hpack (q, 0, additional);
  } else if ((link (q) == null) && (type (q) <= vlist_node) && (shift_amount (q) == 0)) {
	x = q;	/* it's already clean */
  } else {
	x = hpack (q, 0, additional);
  };
  /* begin expansion of Simplify a trivial box */
  /* module 865 */
  /* Here we save memory space in a common case. */
  q = list_ptr (x);
  if (is_char_node (q)) {
	r = link (q);
	if (r != null)
	  if (link (r) == null)
		if (!is_char_node (r))
		  if (type (r) == kern_node) {	/* unneeded italic correction */
			free_node (r, small_node_size);
			link (q) = null;
		  };
  };
  /* end expansion of Simplify a trivial box */
  return x;
};


/* module 868 */

/* The outputs of |fetch| are placed in global variables.
 */

internal_font_number cur_f;/* the |font| field of a |math_char| */
quarterword cur_c;/* the |character| field of a |math_char| */
four_quarters cur_i;/* the |char_info| of a |math_char|,  or a lig/kern instruction */


/* module 866 */

/* It is convenient to have a procedure that converts a |math_char|
 * field to an ``unpacked'' form. The |fetch| routine sets |cur_f|, |cur_c|,
 * and |cur_i| to the font code, character code, and character information bytes of
 * a given noad field. It also takes care of issuing error messages for
 * nonexistent characters; in such cases, |char_exists(cur_i)| will be |false|
 * after |fetch| has acted, and the field will also have been reset to |empty|.
 */
void 
fetch (pointer a) { /* unpack the |math_char| field |a| */
  cur_c = character (a);
  cur_f = fam_fnt (fam (a) + cur_size);
  if (cur_f == null_font) {
	/* begin expansion of Complain about an undefined family and set |cur_i| null */
	/* module 867 */
	print_err ("");
	print_size (cur_size);
	print_char (' ');
	print_int (fam (a));
	zprint_string(" is undefined (character ");
	print_ASCII (qo (cur_c));
	print_char (')');
	help4 ("Somewhere in the math formula just ended, you used the",
		   "stated character from an undefined font family. For example,",
		   "plain TeX doesn't allow \\it or \\sl in subscripts. Proceed,",
		   "and I'll try to forget that I needed that character.");
	error();
	cur_i = null_character;
	math_type (a) = empty;
	/* end expansion of Complain about an undefined family and set |cur_i| null */
  } else {
	if ((qo (cur_c) >= font_bc[cur_f]) && (qo (cur_c) <= font_ec[cur_f])) {
	  cur_i = orig_char_info (cur_f, cur_c);
	} else {
	  cur_i = null_character;
	}
	if (!(char_exists (cur_i))) {
	  char_warning (cur_f, qo (cur_c));
	  math_type (a) = empty;
	};
  };
};

/* module 869 */

/* We need to do a lot of different things, so |mlist_to_hlist| makes two
 * passes over the given mlist.
 * 
 * The first pass does most of the processing: It removes ``mu'' spacing from
 * glue, it recursively evaluates all subsidiary mlists so that only the
 * top-level mlist remains to be handled, it puts fractions and square roots
 * and such things into boxes, it attaches subscripts and superscripts, and
 * it computes the overall height and depth of the top-level mlist so that
 * the size of delimiters for a |left_noad| and a |right_noad| will be known.
 * The hlist resulting from each noad is recorded in that noad's |new_hlist|
 * field, an integer field that replaces the |nucleus| or |thickness|.
 * 
 * The second pass eliminates all noads and inserts the correct glue and
 * penalties between nodes.
 */
#define new_hlist( arg )  mem [ nucleus ( arg )]. cint

/* module 875 */
#define choose_mlist( arg ) { p   =  arg ( q ); arg ( q )  =  null ;}


/* module 878 */

/* Most of the actual construction work of |mlist_to_hlist| is done
 * by procedures with names
 * like |make_fraction|, |make_radical|, etc. To illustrate
 * the general setup of such procedures, let's begin with a couple of
 * simple ones.
 */
void 
make_over (pointer q) {
  info (nucleus (q)) = overbar (clean_box (nucleus (q), cramped_style (cur_style)),
								3 * default_rule_thickness, default_rule_thickness);
  math_type (nucleus (q)) = sub_box;
};


/* module 879 */
void make_under (pointer q) {
  pointer p, x, y; /* temporary registers for box construction */ 
  scaled delta; /* overall height plus depth */ 
  x = clean_box (nucleus (q), cur_style);
  p = new_kern (3 * default_rule_thickness);
  link (x) = p;
  link (p) = fraction_rule (default_rule_thickness);
  y = VPACK (x, 0, additional);
  delta = height (y) + depth (y) + default_rule_thickness;
  height (y) = height (x);
  depth (y) = delta - height (y);
  info (nucleus (q)) = y;
  math_type (nucleus (q)) = sub_box;
};

/* module 880 */
void 
make_vcenter (pointer q) {
  pointer v; /* the box that should be centered vertically */ 
  scaled delta; /* its height plus depth */ 
  v = info (nucleus (q));
  if (type (v) != vlist_node)
	confusion ("vcenter");
  delta = height (v) + depth (v);
  height (v) = axis_height (cur_size) + half (delta);
  depth (v) = delta - height (v);
};


/* module 881 */

/* According to the rules in the \.{DVI} file specifications, we ensure alignment
 * 
 * between a square root sign and the rule above its nucleus by assuming that the
 * baseline of the square-root symbol is the same as the bottom of the rule. The
 * height of the square-root symbol will be the thickness of the rule, and the
 * depth of the square-root symbol should exceed or equal the height-plus-depth
 * of the nucleus plus a certain minimum clearance~|clr|. The symbol will be
 * placed so that the actual clearance is |clr| plus half the excess.
 */
void 
make_radical (pointer q) {
  pointer x, y; /* temporary registers for box construction */ 
  scaled delta, clr; /* dimensions involved in the calculation */ 
  x = clean_box (nucleus (q), cramped_style (cur_style));
  if (cur_style < text_style)	{ /* display style */
	clr = default_rule_thickness + (abs (math_x_height (cur_size)) / 4);
  } else {
	clr = default_rule_thickness;
	clr = clr + (abs (clr) / 4);
  };
  y = var_delimiter (left_delimiter (q), cur_size,
					 height (x) + depth (x) + clr +
					 default_rule_thickness);
  delta = depth (y) - (height (x) + depth (x) + clr);
  if (delta > 0)
	clr = clr + half (delta);
  /* increase the actual clearance */ 
  shift_amount (y) =  -(height (x) + clr);
  link (y) = overbar (x, clr, height (y));
  info (nucleus (q)) = hpack (y, 0, additional);
  math_type (nucleus (q)) = sub_box;
};

/* module 882 */

/* Slants are not considered when placing accents in math mode. The accenter is
 * centered over the accentee, and the accent width is treated as zero with
 * respect to the size of the final box.
 */
void 
make_math_accent (pointer q) {
  pointer p, x, y; /* temporary registers for box construction */ 
  int a; /* address of lig/kern instruction */ 
  quarterword c; /* accent character */ 
  internal_font_number f; /* its font */ 
  four_quarters i; /* its |char_info| */ 
  scaled s; /* amount to skew the accent to the right */ 
  scaled h; /* height of character being accented */ 
  scaled delta; /* space to remove between accent and accentee */ 
  scaled w; /* width of the accentee, not including sub/superscripts */
  fetch (accent_chr (q));
  if (char_exists (cur_i)) {
	i = cur_i;
	c = cur_c;
	f = cur_f;
	/* begin expansion of Compute the amount of skew */
	/* module 885 */
	s = 0;
	if (math_type (nucleus (q)) == math_char) {
	  fetch (nucleus (q));
	  if (char_tag (cur_i) == lig_tag) {
		a = lig_kern_start (cur_f,cur_i);
		cur_i = font_info[a].qqqq;
		if (skip_byte (cur_i) > stop_flag) {
		  a = lig_kern_restart (cur_f,cur_i);
		  cur_i = font_info[a].qqqq;
		};
		loop {
		  if (qo (next_char (cur_i)) == skew_char[cur_f]) {
			if (op_byte (cur_i) >= kern_flag)
			  if (skip_byte (cur_i) <= stop_flag)
				s = char_kern (cur_f,cur_i);
			goto DONE1;
		  };
		  if (skip_byte (cur_i) >= stop_flag)
			goto DONE1;
		  a = a + qo (skip_byte (cur_i)) + 1;
		  cur_i = font_info[a].qqqq;
		};
	  };
	};
  DONE1:
	/* end expansion of Compute the amount of skew */
	x = clean_box (nucleus (q), cramped_style (cur_style));
	w = width (x);
	h = height (x);
	/* begin expansion of Switch to a larger accent if available and appropriate */
	/* module 884 */
	loop {
	  if (char_tag (i) != list_tag)
		goto DONE;
	  y = rem_byte (i);
	  i = orig_char_info (f, y);
	  if (!char_exists (i))
		goto DONE;
	  if (char_width (f, i) > w)
		goto DONE;
	  c = y;
	};
  DONE:
	/* end expansion of Switch to a larger accent if available and appropriate */
	;
	if (h < x_height (f)) {
	  delta = h;
	} else {
	  delta = x_height (f);
	}
	if ((math_type (supscr (q)) != empty) || (math_type (subscr (q)) != empty))
	  if (math_type (nucleus (q)) == math_char) {
		/* begin expansion of Swap the subscript and superscript into box |x| */
		/* module 886 */
		flush_node_list (x);
		x = new_noad();
		mem[nucleus (x)] = mem[nucleus (q)];
		mem[supscr (x)] = mem[supscr (q)];
		mem[subscr (x)] = mem[subscr (q)];
		mem[supscr (q)].hh = empty_field;
		mem[subscr (q)].hh = empty_field;
		math_type (nucleus (q)) = sub_mlist;
		info (nucleus (q)) = x;
		x = clean_box (nucleus (q), cur_style);
		delta = delta + height (x) - h;
		h = height (x);
	  };
	/* end expansion of Swap the subscript and superscript into box |x| */
	y = char_box (f, c);
	shift_amount (y) = s + half (w - width (y));
	width (y) = 0;
	p = new_kern (-delta);
	link (p) = x;
	link (y) = p;
	y = VPACK (y, 0, additional);
	width (y) = width (x);
	if (height (y) < h) {
	  /* begin expansion of Make the height of box |y| equal to |h| */
	  /* module 883 */
	  p = new_kern (h - height (y));
	  link (p) = list_ptr (y);
	  list_ptr (y) = p;
	  height (y) = h;
	  /* end expansion of Make the height of box |y| equal to |h| */
	};
	info (nucleus (q)) = y;
	math_type (nucleus (q)) = sub_box;
  };
};

/* module 887 */

/* The |make_fraction| procedure is a bit different because it sets
 * |new_hlist(q)| directly rather than making a sub-box.
 */
void 
make_fraction (pointer q) {
  pointer p, v, x, y, z; /* temporary registers for box construction */ 
  scaled delta, delta1, delta2, shift_up, shift_down, clr;
  /* dimensions for box calculations */ 
  if (thickness (q) == default_code)
	thickness (q) = default_rule_thickness;
  /* begin expansion of Create equal-width boxes |x| and |z| for the numerator and 
	 denominator, and compute the default amounts |shift_up| and |shift_down| by 
	 which they are displaced from the baseline */
  /* module 888 */
  x = clean_box (numerator (q), num_style (cur_style));
  z = clean_box (denominator (q), denom_style (cur_style));
  if (width (x) < width (z)) {
	x = rebox (x, width (z));
  } else {
	z = rebox (z, width (x));
  }
  if (cur_style < text_style) {	/* display style */
	shift_up = num1 (cur_size);
	shift_down = denom1 (cur_size);
  } else {
	shift_down = denom2 (cur_size);
	if (thickness (q) != 0) {
	  shift_up = num2 (cur_size);
	} else {
	  shift_up = num3 (cur_size);
	}
  };
  /* end expansion of Create equal-width boxes |x| and... */
  if (thickness (q) == 0)
	/* begin expansion of Adjust \(s)|shift_up| and |shift_down| for the 
	   case of no fraction line */
	/* module  889 */
	/* The numerator and denominator must be separated by a certain minimum
	 * clearance, called |clr| in the following program. The difference between
	 * |clr| and the actual clearance is |2delta|.
	 */
	{
	  if (cur_style < text_style) {
		clr = 7 * default_rule_thickness;
	  } else {
		clr = 3 * default_rule_thickness;
	  }
	  delta = half (clr - ((shift_up - depth (x)) - (height (z) - shift_down)));
	  if (delta > 0) {
		shift_up = shift_up + delta;
		shift_down = shift_down + delta;
	  };
	  /* end expansion of Adjust \(s)|shift_up| and |shift_down| for the case of no... */
	} else {
	  /* begin expansion of Adjust \(s)|shift_up| and |shift_down| for the case of a fraction line */
	  /* module 890 */
	  /* In the case of a fraction line, the minimum clearance depends on the actual
	   * thickness of the line.
	   */
	  if (cur_style < text_style) {
		clr = 3 * thickness (q);
	  } else {
		clr = thickness (q);
	  }
	  delta = half (thickness (q));
	  delta1 = clr - ((shift_up - depth (x)) - (axis_height (cur_size) + delta));
	  delta2 =  clr - ((axis_height (cur_size) - delta) - (height (z) - shift_down));
	  if (delta1 > 0)
		shift_up = shift_up + delta1;
	  if (delta2 > 0)
		shift_down = shift_down + delta2;
	  /* end expansion of Adjust \(s)|shift_up| and |shift_down| for the case of a .. */
	};
  /* begin expansion of Construct a vlist box for the fraction, according to |shift_up| 
	 and |shift_down| */
  /* module 891 */
  v = new_null_box();
  type (v) = vlist_node;
  height (v) = shift_up + height (x);
  depth (v) = depth (z) + shift_down;
  width (v) = width (x); /* this also equals |width(z)| */ 
  if (thickness (q) == 0) {
	p = new_kern ((shift_up - depth (x)) - (height (z) - shift_down));
	link (p) = z;
  } else {
	y = fraction_rule (thickness (q));
	p = new_kern ((axis_height (cur_size) - delta) - (height (z) - shift_down));
	link (y) = p;
	link (p) = z;
	p = new_kern ((shift_up - depth (x)) - (axis_height (cur_size) + delta));
	link (p) = y;
  };
  link (x) = p;
  list_ptr (v) = x;
  /* end expansion of Construct a vlist box for the fraction, according to ...*/
  /* begin expansion of Put the \(f)fraction into a box with its delimiters, 
	 and make |new_hlist(q)| point to it */
  /* module 892 */
  if (cur_style < text_style) {
	delta = delim1 (cur_size);
  } else {
	delta = delim2 (cur_size);
  }
  x = var_delimiter (left_delimiter (q), cur_size, delta);
  link (x) = v;
  z = var_delimiter (right_delimiter (q), cur_size, delta);
  link (v) = z;
  new_hlist (q) = hpack (x, 0, additional);
  /* end expansion of Put the \(f)fraction into a box with its delimiters,  ... */
};


/* module 893 */

/* If the nucleus of an |op_noad| is a single character, it is to be
 * centered vertically with respect to the axis, after first being enlarged
 * (via a character list in the font) if we are in display style. The normal
 * convention for placing displayed limits is to put them above and below the
 * operator in display style.
 * 
 * The italic correction is removed from the character if there is a subscript
 * and the limits are not being displayed. The |make_op|
 * routine returns the value that should be used as an offset between
 * subscript and superscript.
 * 
 * After |make_op| has acted, |subtype(q)| will be |limits| if and only if
 * the limits have been set above and below the operator. In that case,
 * |new_hlist(q)| will already contain the desired final box.
 */
scaled 
make_op (pointer q) {
  scaled delta; /* offset between subscript and superscript */ 
  pointer p, v, x, y, z; /* temporary registers for box construction */ 
  quarterword c;
  four_quarters i; /* registers for character examination */ 
  scaled shift_up, shift_down; /* dimensions for box calculation */
  if ((subtype (q) == normal) && (cur_style < text_style))
	subtype (q) = limits;
  if (math_type (nucleus (q)) == math_char) {
	fetch (nucleus (q));
	if ((cur_style < text_style) && (char_tag (cur_i) == list_tag))	{ /* make it larger */
	  c = rem_byte (cur_i);
	  i = orig_char_info (cur_f, c);
	  if (char_exists (i)) {
		cur_c = c;
		cur_i = i;
		character (nucleus (q)) = c;
	  };
	};
	delta = char_italic (cur_f, cur_i);
	x = clean_box (nucleus (q), cur_style);
	if ((math_type (subscr (q)) != empty) && (subtype (q) != limits))
	  width (x) = width (x) - delta; /* remove italic correction */ 
	shift_amount (x) =half (height (x) - depth (x)) - axis_height (cur_size); /* center vertically */ 
	math_type (nucleus (q)) = sub_box; info (nucleus (q)) = x;
  } else {
	delta = 0;
  }
  if (subtype (q) == limits) {
	/* begin expansion of Construct a box with limits above and below it, skewed by |delta| */
	/* module 894 */
	/* The following program builds a vlist box |v| for displayed limits. The
	 * width of the box is not affected by the fact that the limits may be skewed.
	 */
	x = clean_box (supscr (q), sup_style (cur_style));
	y = clean_box (nucleus (q), cur_style);
	z = clean_box (subscr (q), sub_style (cur_style));
	v = new_null_box();
	type (v) = vlist_node;
	width (v) = width (y);
	if (width (x) > width (v))
	  width (v) = width (x);
	if (width (z) > width (v))
	  width (v) = width (z);
	x = rebox (x, width (v));
	y = rebox (y, width (v));
	z = rebox (z, width (v));
	shift_amount (x) = half (delta);
	shift_amount (z) = -shift_amount (x);
	height (v) = height (y);
	depth (v) = depth (y);
	/* begin expansion of Attach the limits to |y| and adjust |height(v)|, |depth(v)| to account for their presence */
	/* module 895 */
	/* We use |shift_up| and |shift_down| in the following program for the
	 * amount of glue between the displayed operator |y| and its limits |x| and
	 * |z|. The vlist inside box |v| will consist of |x| followed by |y| followed
	 * by |z|, with kern nodes for the spaces between and around them.
	 */
	if (math_type (supscr (q)) == empty) {
	  free_node (x, box_node_size);
	  list_ptr (v) = y;
	} else {
	  shift_up = big_op_spacing3 - depth (x);
	  if (shift_up < big_op_spacing1)
		shift_up = big_op_spacing1;
	  p = new_kern (shift_up);
	  link (p) = y;
	  link (x) = p;
	  p = new_kern (big_op_spacing5);
	  link (p) = x;
	  list_ptr (v) = p;
	  height (v) =
		height (v) + big_op_spacing5 + height (x) +
		depth (x) + shift_up;
	};
	if (math_type (subscr (q)) == empty) {
	  free_node (z, box_node_size);
	} else {
	  shift_down = big_op_spacing4 - height (z);
	  if (shift_down < big_op_spacing2)
		shift_down = big_op_spacing2;
	  p = new_kern (shift_down);
	  link (y) = p;
	  link (p) = z;
	  p = new_kern (big_op_spacing5);
	  link (z) = p;
	  depth (v) = depth (v) + big_op_spacing5 + height (z) + depth (z) + shift_down;
	  /* end expansion of Attach the limits to |y| and adjust |height(v)|, |depth(v)| to account for their presence */
	};
	new_hlist (q) = v;
	/* end expansion of Construct a box with limits above and below it, skewed by |delta| */
  };
  return delta;
};

/* module 896 */

/* A ligature found in a math formula does not create a |ligature_node|, because
 * there is no question of hyphenation afterwards; the ligature will simply be
 * stored in an ordinary |char_node|, after residing in an |ord_noad|.
 * 
 * The |math_type| is converted to |math_text_char| here if we would not want to
 * apply an italic correction to the current character unless it belongs
 * to a math font (i.e., a font with |space=0|).
 * 
 * No boundary characters enter into these ligatures.
 */
void
make_ord (pointer q) {
  int a; /* address of lig/kern instruction */ 
  pointer p, r; /* temporary registers for list manipulation */ 
 RESTART:
  if (math_type (subscr (q)) == empty) {
	if (math_type (supscr (q)) == empty) {
	  if (math_type (nucleus (q)) == math_char) {
		p = link (q);
		if (p != null) {
		  if ((type (p) >= ord_noad) && (type (p) <= punct_noad)) {
			if (math_type (nucleus (p)) == math_char) {
			  if (fam (nucleus (p)) == fam (nucleus (q))) {
				math_type (nucleus (q)) = math_text_char;
				fetch (nucleus (q));
				if (char_tag (cur_i) == lig_tag) {
				  a = lig_kern_start (cur_f,cur_i);
				  cur_c = character (nucleus (p));
				  cur_i = font_info[a].qqqq;
				  if (skip_byte (cur_i) > stop_flag) {
					a = lig_kern_restart (cur_f,cur_i);
					cur_i = font_info[a].qqqq;
				  };
				  loop {
					/* begin expansion of If instruction |cur_i| is a kern with |cur_c|, 
					   attach the kern after~|q|; or if it is a ligature with |cur_c|, 
					   combine noads |q| and~|p| appropriately; then |return| if the cursor 
					   has moved past a noad, or |goto restart| */
					/* module 897 */
					/* Note that a ligature between an |ord_noad| and another kind of noad
					 * is replaced by an |ord_noad|, when the two noads collapse into one.
					 * But we could make a parenthesis (say) change shape when it follows
					 * certain letters. Presumably a font designer will define such
					 * ligatures only when this convention makes sense.
					 * 
					 * \chardef\?='174 % vertical line to indicate character retention
					 */
				    if (next_char (cur_i) == cur_c) {
				      if (skip_byte (cur_i) <= stop_flag) {
						if (op_byte (cur_i) >= kern_flag) {
						  p =  new_kern (char_kern (cur_f,cur_i));
						  link (p) = link (q);
						  link (q) = p;
						  return;
						} else {
						  check_interrupt; /* allow a way out of infinite ligature loop */
						  switch (op_byte (cur_i)) {
						  case qi (1):
						  case qi (5):
							character (nucleus (q)) = rem_byte (cur_i);
							break;
						  case qi (2):  /* \.{=:\?}, \.{=:\?>} */ 
						  case qi (6):
							character (nucleus (p)) = rem_byte (cur_i);
							break;
						  case qi (3): /* \.{\?=:}, \.{\?=:>} */ 
						  case qi (7):
						  case qi (11):
							r = new_noad(); /* \.{\?=:\?}, \.{\?=:\?>}, \.{\?=:\?>>} */
							character(nucleus(r)) = rem_byte (cur_i);
							fam (nucleus (r)) = fam (nucleus (q));
							link (q) = r;
							link (r) = p;
							if (op_byte (cur_i) <qi (11)) {
							  math_type (nucleus(r)) = math_char;
						    } else {
							  math_type (nucleus(r)) = math_text_char; /* prevent combination */
							};
							break;
						  default:
							link (q) = link (p);
							character (nucleus (q))= rem_byte (cur_i); /* \.{=:} */
							mem[subscr(q)] = mem[subscr (p)];
							mem[supscr (q)] =mem[supscr (p)];
							free_node (p, noad_size);
						  };
						  if (op_byte (cur_i) > qi (3))
							return;
						  math_type (nucleus (q)) = math_char;
						  goto RESTART;
						}
						/* end expansion of If instruction |cur_i| is a kern...*/
					  }
					};
				    if (skip_byte (cur_i) >= stop_flag)
				      return;
				    a = a + qo (skip_byte (cur_i)) + 1;
				    cur_i = font_info[a].qqqq;
				  };
				};
			  };
			}
		  }
		}
	  };
	}
  }
};


/* module 900 */

/* The purpose of |make_scripts(q,delta)| is to attach the subscript and/or
 * superscript of noad |q| to the list that starts at |new_hlist(q)|,
 * given that subscript and superscript aren't both empty. The superscript
 * will appear to the right of the subscript by a given distance |delta|.
 * 
 * We set |shift_down| and |shift_up| to the minimum amounts to shift the
 * baseline of subscripts and superscripts based on the given nucleus.
 */
void 
make_scripts (pointer q, scaled delta) {
  pointer p, x, y, z; /* temporary registers for box construction */ 
  scaled shift_up, shift_down, clr; /* dimensions in the calculation */ 
  small_number t; /* subsidiary size code */ 
  p = new_hlist (q);
  if (is_char_node (p)) {
	shift_up = 0;
	shift_down = 0;
  } else {
	z = hpack (p, 0, additional);
	if (cur_style < script_style) {
	  t = script_size;
	} else {
	  t = script_script_size;
	}
	shift_up = height (z) - sup_drop (t);
	shift_down = depth (z) + sub_drop (t);
	free_node (z, box_node_size);
  };
  if (math_type (supscr (q)) == empty) {
	/* begin expansion of Construct a subscript box |x| when there is no superscript */
	/* module 901 */
	/* When there is a subscript without a superscript, the top of the subscript
	 * should not exceed the baseline plus four-fifths of the x-height.
	 */
	x = clean_box (subscr (q), sub_style (cur_style));
	width (x) = width (x) + script_space;
	if (shift_down < sub1 (cur_size))
	  shift_down = sub1 (cur_size);
	clr = height (x) - (abs (math_x_height (cur_size) * 4) / 5);
	if (shift_down < clr)
	  shift_down = clr;
	shift_amount (x) = shift_down;
	/* end expansion of Construct a subscript box |x| when there is no superscript */
  } else {
	/* begin expansion of Construct a superscript box |x| */
	/* module 902 */
	/* The bottom of a superscript should never descend below the baseline plus
	 * one-fourth of the x-height.
	 */
	x = clean_box (supscr (q), sup_style (cur_style));
	width (x) = width (x) + script_space;
	if (odd (cur_style)) {
	  clr = sup3 (cur_size);
	} else if (cur_style < text_style) {
	  clr = sup1 (cur_size);
	} else {
	  clr = sup2 (cur_size);
	};
	if (shift_up < clr)
	  shift_up = clr;
	clr = depth (x) + (abs (math_x_height (cur_size)) / 4);
	if (shift_up < clr)
	  shift_up = clr;
	/* end expansion of Construct a superscript box |x| */
	if (math_type (subscr (q)) == empty) {
	  shift_amount (x) = -shift_up;
	} else {
	  /* begin expansion of Construct a sub/superscript combination box |x|, 
		 with the superscript offset by |delta| */
	  /* module 903 */
	  /* When both subscript and superscript are present, the subscript must be
	   * separated from the superscript by at least four times |default_rule_thickness|.
	   * If this condition would be violated, the subscript moves down, after which
	   * both subscript and superscript move up so that the bottom of the superscript
	   * is at least as high as the baseline plus four-fifths of the x-height.
	   */
	  y = clean_box (subscr (q), sub_style (cur_style));
	  width (y) = width (y) + script_space;
	  if (shift_down < sub2 (cur_size))
		shift_down = sub2 (cur_size);
	  clr = 4 * default_rule_thickness - ((shift_up - depth (x)) - (height (y) - shift_down));
	  if (clr > 0) {
		shift_down = shift_down + clr;
		clr = (abs (math_x_height (cur_size) * 4) / 5) - (shift_up - depth (x));
		if (clr > 0) {
		  shift_up = shift_up + clr;
		  shift_down = shift_down - clr;
		};
	  };
	  shift_amount (x) = delta; /* superscript is |delta| to the right of the subscript */
	  p = new_kern ((shift_up - depth (x)) - (height (y) - shift_down));
	  link (x) = p;
	  link (p) = y;
	  x = VPACK (x, 0, additional);
	  shift_amount (x) = shift_down;
	  /* end expansion of Construct a sub/superscript combination box |x|, ...*/
	};
  };
  if (new_hlist (q) == null) {
	new_hlist (q) = x;
  } else {
	p = new_hlist (q);
	while (link (p) != null)
	  p = link (p);
	link (p) = x;
  };
};

/* module 908 */

/* The inter-element spacing in math formulas depends on a $8\times8$ table that
 * \TeX\ preloads as a 64-digit string. The elements of this string have the
 * following significance:
 * $$\vbox{\halign{#\hfil\cr
 * \.0 means no space;\cr
 * \.1 means a conditional thin space   (\.{\\nonscript\\mskip\\thinmuskip});\cr
 * \.2 means a thin space               (\.{\\mskip\\thinmuskip});\cr
 * \.3 means a conditional medium space (\.{\\nonscript\\mskip\\medmuskip});\cr
 * \.4 means a conditional thick space  (\.{\\nonscript\\mskip\\thickmuskip});\cr
 * \.* means an impossible case.\cr}}$$
 * This is all pretty cryptic, but {\sl The \TeX book\/} explains what is
 * supposed to happen, and the string makes it happen.
 * 
 * A global variable |magic_offset| is computed so that if |a| and |b| are
 * in the range |ord_noad..inner_noad|, then |str_pool[a*8+b+magic_offset]|
 * is the digit for spacing between noad types |a| and |b|.
 * 
 * If \PASCAL\ had provided a good way to preload constant arrays, this part of
 * the program would not have been so strange. 
 *
 * In C, we could do what Knuth proposes above. However, there is no realy advantage
 * to doing so. But, it is possible to get rid of the 'magic_offset' weirdness,
 * because the conversion did not use tangle with it's (somewhat confusing) string
 * handling.
 */
/* integer magic_offset;*/ /* used to find inter-element spacing */

/* module 906 */

/* The |make_left_right| function constructs a left or right delimiter of
 * the required size and returns the value |open_noad| or |close_noad|. The
 * |right_noad| and |left_noad| will both be based on the original |style|,
 * so they will have consistent sizes.
 * 
 * We use the fact that |right_noad-left_noad=close_noad-open_noad|.
 */
small_number 
make_left_right (pointer q, small_number style,	scaled max_d, scaled max_h) {
  scaled delta, delta1, delta2; /* dimensions used in the calculation */ 
  cur_style = style;
  /* Set up the values... */
  set_cur_size_and_mu;
  delta2 = max_d + axis_height (cur_size);
  delta1 = max_h + max_d - delta2;
  if (delta2 > delta1)
	delta1 = delta2;
  /* |delta1| is max distance from axis */ 
  delta = (delta1 / 500) * delimiter_factor;
  delta2 = delta1 + delta1 - delimiter_shortfall;
  if (delta < delta2)
	delta = delta2;
  new_hlist (q) = var_delimiter (delimiter (q), cur_size, delta);
  return type (q) - (left_noad - open_noad);
  /* |open_noad| or |close_noad| */ 
};


/* module 873 */
#define convert_final_bin_to_ord if (r_type == bin_noad) {type (r) = ord_noad; }

/* module 870 */

/* Here is the overall plan of |mlist_to_hlist|, and the list of its
 * local variables.
 */

void 
mlist_to_hlist (void) {
  pointer mlist; /* beginning of the given list */ 
  boolean penalties; /* should penalty nodes be inserted? */ 
  small_number style; /* the given style */ 
  small_number save_style; /* holds |cur_style| during recursion */ 
  pointer q; /* runs through the mlist */ 
  pointer r; /* the most recent noad preceding |q| */ 
  small_number r_type; /* the |type| of noad |r|, or |op_noad| if |r=null| */
  small_number t; /* the effective |type| of noad |q| during the second pass */
  pointer p, x, y, z; /* temporary registers for list construction */ 
  int pen; /* a penalty to be inserted */ 
  small_number s; /* the size of a noad to be deleted */ 
  scaled max_h, max_d; /* maximum height and depth of the list translated so far */
  scaled delta;  /* offset between subscript and superscript */ 
  p =null; x=null; /*TH -Wall*/
  mlist = cur_mlist;
  penalties = mlist_penalties;
  style = cur_style; /* tuck global parameters away as local variables */ 
  q = mlist;
  r = null;
  r_type = op_noad;
  max_h = 0;
  max_d = 0;
  /* Set up the values of |cur_size| and |cur_mu|, based on |cur_style| */
  set_cur_size_and_mu;
  while (q != null) {
	/* begin expansion of Process node-or-noad |q| as much as possible in preparation 
	   for the second pass of |mlist_to_hlist|, then move to the next item in the mlist */
	/* module 871 */
	/* We use the fact that no character nodes appear in an mlist, hence
	 * the field |type(q)| is always present.
	 */
	/* begin expansion of Do first-pass processing based on |type(q)|; |goto done_with_noad| 
	   if a noad has been fully processed, |goto check_dimensions| if it has been translated 
	   into |new_hlist(q)|, or |goto done_with_node| if a node has been fully processed */
	/* module 872 */
	/* One of the things we must do on the first pass is change a |bin_noad| to
	 * an |ord_noad| if the |bin_noad| is not in the context of a binary operator.
	 * The values of |r| and |r_type| make this fairly easy.
	 */
  RESWITCH:
	delta = 0;
	switch (type (q)) {
	case bin_noad:
	  switch (r_type) {
	  case bin_noad:
	  case op_noad:
	  case rel_noad:
	  case open_noad:
	  case punct_noad:
	  case left_noad:
		type (q) = ord_noad;
		goto RESWITCH;
		break;
	  default:
		do_nothing;
	  };
	  do_something;
	  break;
	case rel_noad:
	case close_noad:
	case punct_noad:
	case right_noad:
	  /* Convert \(a)a final |bin_noad| to an |ord_noad| */
	  convert_final_bin_to_ord;
	  if (type (q) == right_noad)
		goto DONE_WITH_NOAD;
	  do_something;
	  break;
	  /* begin expansion of Cases for noads that can follow a |bin_noad| */
	  /* module 877 */
	case left_noad:
	  do_something;
	  goto DONE_WITH_NOAD;
	case fraction_noad:
	  make_fraction (q);
	  goto CHECK_DIMENSIONS;
	case op_noad:
	  delta = make_op (q);
	  if (subtype (q) == limits)
		goto CHECK_DIMENSIONS;
	  do_something;
	  break;
	case ord_noad:
	  make_ord (q);
	  break;
	case open_noad:
	case inner_noad:
	  do_something;
	  break;
	case radical_noad:
	  make_radical (q);
	  break;
	case over_noad:
	  make_over (q);
	  break;
	case under_noad:
	  make_under (q);
	  break;
	case accent_noad:
	  make_math_accent (q);
	  break;
	case vcenter_noad:
	  make_vcenter (q);
	  break;
	  /* end expansion of Cases for noads that can follow a |bin_noad| */
	  /* begin expansion of Cases for nodes that can appear in an mlist, after which we |goto done_with_node| */
	  /* module 874 */
	case style_node:
	  cur_style = subtype (q);
	  /* Set up the values of |cur_size| and |cur_mu|, based on |cur_style| */
	  set_cur_size_and_mu;
	  goto DONE_WITH_NODE;
	  break;
	case choice_node:
	  /* begin expansion of Change this node to a style node followed by the correct choice, then |goto done_with_node| */
	  /* module 875 */
	  switch (cur_style / 2) {
	  case 0: /* |display_style=0| */
		choose_mlist(display_mlist);
		break;
	  case 1:  /* |text_style=2| */  
		choose_mlist (text_mlist);
		break;
	  case 2:  /* |script_style=4| */
		choose_mlist (script_mlist);
		break;
	  case 3:  /* |script_script_style=6| */ 
		choose_mlist (script_script_mlist); 
	  }; /* there are no other cases */
	  flush_node_list (display_mlist (q));
	  flush_node_list (text_mlist (q));
	  flush_node_list (script_mlist (q));
	  flush_node_list (script_script_mlist (q));
	  type (q) = style_node;
	  subtype (q) = cur_style;
	  width (q) = 0;
	  depth (q) = 0;
	  if (p != null) {
		z = link (q);
		link (q) = p;
		while (link (p) != null)
		  p = link (p);
		link (p) = z;
	  };
	  goto DONE_WITH_NODE;
	  /* end expansion of Change this node to a style node followed by the correct choice, then |goto done_with_node| */
	case ins_node:
	case mark_node:
	case adjust_node:
	case whatsit_node:
	case penalty_node:
	case disc_node:
	  do_something;
	  goto DONE_WITH_NODE;
	case rule_node:
	  if (height (q) > max_h)
		max_h = height (q);
	  if (depth (q) > max_d)
		max_d = depth (q);
	  goto DONE_WITH_NODE;
	case glue_node:
	  /* begin expansion of Convert \(m)math glue to ordinary glue */
	  /* module 876 */
	  /* Conditional math glue (`\.{\\nonscript}') results in a |glue_node|
	   * pointing to |zero_glue|, with |subtype(q)=cond_math_glue|; in such a case
	   * the node following will be eliminated if it is a glue or kern node and if the
	   * current size is different from |text_size|. Unconditional math glue
	   * (`\.{\\muskip}') is converted to normal glue by multiplying the dimensions
	   * by |cur_mu|.
	   */
	  if (subtype (q) == mu_glue) {
		x = glue_ptr (q);
		y = math_glue (x, cur_mu);
		delete_glue_ref (x);
		glue_ptr (q) = y;
		subtype (q) = normal;
	  } else if ((cur_size != text_size) && (subtype (q) == cond_math_glue)) {
		p = link (q);
		if (p != null)
		  if ((type (p) == glue_node) || (type (p) == kern_node)) {
			link (q) = link (p);
			link (p) = null;
			flush_node_list (p);
		  };
		/* end expansion of Convert \(m)math glue to ordinary glue */
	  };
	  goto DONE_WITH_NODE;
	case kern_node:
	  math_kern (q, cur_mu);
	  goto DONE_WITH_NODE;
	default:
	  confusion ("mlist1");
	};
	/* begin expansion of Convert \(n)|nucleus(q)| to an hlist and attach the sub/superscripts */
	/* module 898 */
	/* When we get to the following part of the program, we have ``fallen through''
	 * from cases that did not lead to |check_dimensions| or |done_with_noad| or
	 * |done_with_node|. Thus, |q|~points to a noad whose nucleus may need to be
	 * converted to an hlist, and whose subscripts and superscripts need to be
	 * appended if they are present.
	 * 
	 * If |nucleus(q)| is not a |math_char|, the variable |delta| is the amount
	 * by which a superscript should be moved right with respect to a subscript
	 * when both are present.
	 */
	switch (math_type (nucleus (q))) {
	case math_char:
	case math_text_char:
	  /* begin expansion of Create a character node |p| for |nucleus(q)|, possibly followed 
		 by a kern node for the italic correction, and set |delta| to the italic correction 
		 if a subscript is present */
	  /* module 899 */
	  fetch (nucleus (q));
	  if (char_exists (cur_i)) {
		delta = char_italic (cur_f, cur_i);
		p = new_character (cur_f, qo (cur_c));
		if ((math_type (nucleus (q)) == math_text_char) && (space (cur_f) != 0))
		  delta = 0;
		/* no italic correction in mid-word of text font */
		if ((math_type (subscr (q)) == empty) && (delta != 0)) {
		  link (p) = new_kern (delta);
		  delta = 0;
		};
	  } else {
		p = null;
	  }
	  /* end expansion of Create a character node |p| for |nucleus(q)|,... */
	  break;
	case empty:
	  p = null;
	  break;
	case sub_box:
	  p = info (nucleus (q));
	  break;
	case sub_mlist:
	  cur_mlist = info (nucleus (q));
	  save_style = cur_style;
	  mlist_penalties = false;
	  mlist_to_hlist(); /* recursive call */ 
	  cur_style = save_style;
	  /* Set up the values... */
	  set_cur_size_and_mu;
	  p = hpack (link (temp_head), 0, additional);
	  break;
	default:
	  confusion ("mlist2");
	};
	new_hlist (q) = p;
	if ((math_type (subscr (q)) == empty) && (math_type (supscr (q)) == empty))
	  goto CHECK_DIMENSIONS;
	make_scripts (q, delta);
	  /* end expansion of Convert \(n)|nucleus(q)| to an hlist and attach the sub/superscripts */
	  /* end expansion of Do first-pass processing based on |type(q)|; |goto done_with_noad| if.. */
  CHECK_DIMENSIONS:
	z = hpack (new_hlist (q), 0, additional);
	if (height (z) > max_h)
	  max_h = height (z);
	if (depth (z) > max_d)
	  max_d = depth (z);
	free_node (z, box_node_size);
  DONE_WITH_NOAD:
	r = q;
	r_type = type (r);
	if (r_type == right_noad) {
	  r_type = left_noad;
	  cur_style = style;
	  /* Set up the values... */
	  set_cur_size_and_mu;
	};
  DONE_WITH_NODE:
	q = link (q);
	/* end expansion of Process node-or-noad |q| as much as possible .. */
  };
  /* Convert \(a)a final |bin_noad| to an |ord_noad| */
  convert_final_bin_to_ord;
  /* begin expansion of Make a second pass over the mlist, removing all noads and
	 inserting the proper spacing and penalties */
  /* module 904 */
  /* We have now tied up all the loose ends of the first pass of |mlist_to_hlist|.
   * The second pass simply goes through and hooks everything together with the
   * proper glue and penalties. It also handles the |left_noad| and |right_noad| that
   * might be present, since |max_h| and |max_d| are now known. Variable |p| points
   * to a node at the current end of the final hlist.
   */
  p = temp_head;
  link (p) = null;
  q = mlist;
  r_type = 0;
  cur_style = style;
  /* Set up the values of |cur_size| and |cur_mu|, based on |cur_style| */
  set_cur_size_and_mu ;
  while (q != null) {
	/* begin expansion of If node |q| is a style node, change the style and |goto delete_q|;
	   otherwise if it is not a noad, put it into the hlist, advance |q|, and |goto done|;
	   otherwise set |s| to the size of noad |q|, set |t| to the associated type (|ord_noad.. inner_noad|),
	   and set |pen| to the associated penalty */
	/* module 905 */
	/* Just before doing the big |case| switch in the second pass, the program
	 * sets up default values so that most of the branches are short.
	 */
	t = ord_noad;
	s = noad_size;
	pen = inf_penalty;
	switch (type (q)) {
	case op_noad:
	case open_noad:
	case close_noad:
	case punct_noad:
	case inner_noad:
	  t = type (q);
	  break;
	case bin_noad:
	  t = bin_noad;
	  pen = bin_op_penalty;
	  break;
	case rel_noad:
	  t = rel_noad;
	  pen = rel_penalty;
	  break;
	case ord_noad:
	case vcenter_noad:
	case over_noad:
	case under_noad:
	  do_something;
	  break;
	case radical_noad:
	  s = radical_noad_size;
	  break;
	case accent_noad:
	  s = accent_noad_size;
	  break;
	case fraction_noad:
	  t = inner_noad;
	  s = fraction_noad_size;
	  break;
	case left_noad:
	case right_noad:
	  t = make_left_right (q, style, max_d, max_h);
	  break;
	case style_node:
	  /* begin expansion of Change the current style and |goto delete_q| */
	  /* module 907 */
	  cur_style = subtype (q);
	  s = style_node_size;
	  /* Set up the values of |cur_size| and |cur_mu|, based on |cur_style| */
	  set_cur_size_and_mu ;
	  goto DELETE_Q;
	  /* end expansion of Change the current style and |goto delete_q| */
	case whatsit_node:
	case penalty_node:
	case rule_node:
	case disc_node:
	case adjust_node:
	case ins_node:
	case mark_node:
	case glue_node:
	case kern_node:
	  link (p) = q;
	  p = q;
	  q = link (q);
	  link (p) = null;
	  goto DONE;
	default:
	  confusion ("mlist3");
	};
	/* end expansion of If node |q| is a style node, change the style and |goto delete_...*/
	/* begin expansion of Append inter-element spacing based on |r_type| and |t| */
	/* module 910 */
	if (r_type > 0)	{ /* not the first noad */
	  /*switch (so (str_pool[r_type * 8 + t + magic_offset]))*/
	  char *math_spacing ="0234000122*4000133**3**344*0400400*000000234000111*1111112341011"; 
	  switch (str_pool[(integer)math_spacing[(r_type-ord_noad)*8-(t-ord_noad)]]) {
	  case '0':
		x = 0;
		break;
	  case '1':
		if (cur_style < script_style) {
		  x = thin_mu_skip_code;
		} else {
		  x = 0;
		}
		break;
	  case '2':
		x = thin_mu_skip_code;
		break;
	  case '3':
		if (cur_style < script_style) {
		  x = med_mu_skip_code;
		} else {
		  x = 0;
		}
		break;
	  case '4':
		if (cur_style < script_style) {
		  x = thick_mu_skip_code;
		} else {
		  x = 0;
		}
		break;
	  default:
		confusion ("mlist4");
	  };
	  if (x != 0) {
		y = math_glue (glue_par (x), cur_mu);
		z = new_glue (y);
		glue_ref_count (y) = null;
		link (p) = z;
		p = z;
		subtype (z) = x + 1; /* store a symbolic subtype */ 
	  };
	};
	/* end expansion of Append inter-element spacing based on |r_type| and |t| */

	/* begin expansion of Append any |new_hlist| entries for |q|, and any appropriate penalties */
	/* module 911 */
	/* We insert a penalty node after the hlist entries of noad |q| if |pen|
	 * is not an ``infinite'' penalty, and if the node immediately following |q|
	 * is not a penalty node or a |rel_noad| or absent entirely.
	 */
	if (new_hlist (q) != null) {
	  link (p) = new_hlist (q);
	  do {
		p = link (p);
	  } while (link (p) != null);
	};
	if (penalties)
	  if (link (q) != null)
		if (pen < inf_penalty) {
		  r_type = type (link (q));
		  if (r_type != penalty_node)
			if (r_type != rel_noad) {
			  z = new_penalty (pen);
			  link (p) = z;
			  p = z;
			};
		};
	/* end expansion of Append any |new_hlist| entries for |q|, and any appropriate penalties */
	if (type (q) == right_noad)
	  t = open_noad;
	r_type = t;
  DELETE_Q:
	r = q;
	q = link (q);
	free_node (r, s);
  DONE: 
	do_nothing; 
  };
  /* end expansion of Make a second pass over the mlist, removing all noads and inserting the proper spacing and penalties */
};

