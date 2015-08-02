#ifndef TYPELIST_HH
# define TYPELIST_HH

# include "shader/shader_factory_policy.hh"

# define TYPELIST_1(T1) TypeList<T1, NullType>
# define TYPELIST_2(T0, T1) TypeList<T0, TYPELIST_1(T1)>
# define TYPELIST_3(T0, T1, T2) TypeList<T0, TYPELIST_2(T1, T2)>
# define TYPELIST_4(T0, T1, T2, T3) TypeList<T0, TYPELIST_3(T1, T2, T3)>
# define TYPELIST_5(T0, T1, T2, T3, T4) TypeList<T0, TYPELIST_4(T1, T2, T3, T4)>
# define TYPELIST_6(T0, T1, T2, T3, T4, T5) TypeList<T0, TYPELIST_5(T1, T2, T3, T4, T5)>
# define TYPELIST_7(T0, T1, T2, T3, T4, T5, T6) TypeList<T0, TYPELIST_6(T1, T2, T3, T4, T5, T6)>
# define TYPELIST_8(T0, T1, T2, T3, T4, T5, T6, T7) TypeList<T0, TYPELIST_7(T1, T2, T3, T4, T5, T6, T7)>
# define TYPELIST_9(T0, T1, T2, T3, T4, T5, T6, T7, T8) TypeList<T0, TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8)>
# define TYPELIST_10(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9) TypeList<T0, TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9)>
# define TYPELIST_11(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T0, TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10)>
# define TYPELIST_12(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) TypeList<T0, TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11)>
# define TYPELIST_13(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) TypeList<T0, TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12)>
# define TYPELIST_14(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) TypeList<T0, TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)>
# define TYPELIST_15(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14) TypeList<T0, TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14)>
# define TYPELIST_16(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15) TypeList<T0, TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15)>
# define TYPELIST_17(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16) TypeList<T0, TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16)>
# define TYPELIST_18(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17) TypeList<T0, TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17)>
# define TYPELIST_19(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18) TypeList<T0, TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18)>

struct NullType {};

template <SFP H, typename T>
struct TypeList {
  SFP head = H;
  typedef T tail;
};

/* search functionality */
template <typename TList, SFP T> struct indexOf;

template <SFP T>
struct indexOf<NullType, T> { enum { val = -1 }; };

template <SFP T, typename Tail>
struct indexOf<TypeList<T, Tail>, T> { enum { val = 0 }; };

template <SFP Head, typename Tail, SFP T>
struct indexOf<TypeList<Head, Tail>, T> {
  enum { temp  = indexOf<Tail, T>::val };
  enum { val = temp == -1 ? -1 : 1 + temp };
};

#endif
