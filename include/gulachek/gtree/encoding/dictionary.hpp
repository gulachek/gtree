#ifndef GULACHEK_GTREE_ENCODING_DICTIONARY_HPP
#define GULACHEK_GTREE_ENCODING_DICTIONARY_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/preprocessor/seq/for_each.hpp>


namespace gulachek::gtree
{
	template <typename T, typename Enable = void>
	struct dictionary_encoding
	{
		/*
		typedef T type;

		template <typename Tree>
		static void encode(
				const T &val,
				Tree &tr
				)
		{
		}

		template <typename Tree>
		static void decode(
				const std::string &prop,
				const Tree &tr,
				T &val
				)
		{
		}
		*/
	};

	template <typename T>
	struct uses_value<T,
		enable_t<typename dictionary_encoding<T>::type>> :
		std::false_type {};

	template <typename T>
	struct uses_children<T,
		enable_t<typename dictionary_encoding<T>::type>> :
		std::true_type {};

	template <typename Tree, typename T,
					 typename dictionary_encoding<T>::type* = nullptr>
	void decode(const Tree &tr, T &val)
	{
		using encoding = dictionary_encoding<T>;

		for (std::size_t i = 0; i < tr.child_count(); i++)
		{
			std::string prop;
			decode(tr.child(i), prop);

			encoding::decode(prop, tr.child(i), val);
		}
	}

	template <typename Tree, typename T,
					 typename dictionary_encoding<T>::type* = nullptr>
	void encode(const T &val, Tree &tr)
	{
		using encoding = dictionary_encoding<T>;
		encoding::encode(val, tr);
	}
}

#define _GULACHEK_STRUCT_IMPL_IMPL(                                   \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPLE_SIZE)                                 \
                                                                                \
    BOOST_PP_SEQ_FOR_EACH_R(                                                    \
        1,                                                                      \
        BOOST_FUSION_DEFINE_STRUCT_ATTR_I,                                      \
        ATTRIBUTE_TUPLE_SIZE,                                                   \
        ATTRIBUTES_SEQ)                                                         \
                                                                                \

#define _GULACHEK_DEFINE_NONEMPTY_STRUCT_IMPL(                               \
    NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPLE_SIZE)                                 \
                                                                                \
        _GULACHEK_STRUCT_IMPL_IMPL(                                   \
            NAME, BOOST_PP_SEQ_TAIL(ATTRIBUTES_SEQ), ATTRIBUTE_TUPLE_SIZE)      \

#define _GULACHEK_DEFINE_PMAP_IMPL(                                   \
    NAMESPACE_SEQ, NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPLE_SIZE)                  \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_BEGIN(NAMESPACE_SEQ)         \
                                                                                \
    struct NAME                                                                 \
    {                                                                           \
        typedef NAME self_type;                                                 \
                                                                                \
        BOOST_PP_IF(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(ATTRIBUTES_SEQ)),            \
            _GULACHEK_DEFINE_NONEMPTY_STRUCT_IMPL,                           \
            BOOST_FUSION_DEFINE_EMPTY_STRUCT_IMPL)(                             \
                NAME, ATTRIBUTES_SEQ, ATTRIBUTE_TUPLE_SIZE)                     \
    };                                                                          \
                                                                                \
    BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DEFINITION_END(NAMESPACE_SEQ)

#define _GULACHEK_DECODE_PROP(R, DATA, ELEM) \
	else if (prop == BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(2,1,ELEM))) \
{ \
	if constexpr (is_pure_container<BOOST_PP_TUPLE_ELEM(2,0,ELEM)>::value) \
	{ \
	\
		gulachek::gtree::decode(tr, val. BOOST_PP_TUPLE_ELEM(2,1,ELEM) ); \
	} \
	else \
	{ \
		gulachek::gtree::decode(tr.child(0), val. BOOST_PP_TUPLE_ELEM(2,1,ELEM) ); \
	} \
}

#define _GULACHEK_ENCODE_PROP(R, DATA, I, ELEM) \
gulachek::gtree::encode(BOOST_PP_STRINGIZE(\
			BOOST_PP_TUPLE_ELEM(2,1,ELEM)), tr.child(I)); \
if constexpr (is_pure_container<BOOST_PP_TUPLE_ELEM(2,0,ELEM)>::value) \
{ \
\
	gulachek::gtree::encode(val.BOOST_PP_TUPLE_ELEM(2,1,ELEM), tr.child(I)); \
} \
else \
{ \
	tr.child(I).child_count(1); \
	gulachek::gtree::encode(val.BOOST_PP_TUPLE_ELEM(2,1,ELEM), tr.child(I).child(0)); \
} \

#define _GULACHEK_PMAP_ADAPT_STRUCT(NAME, ATTR_SEQ) \
	namespace gulachek::gtree \
{ \
	template <> \
	struct dictionary_encoding<NAME> \
	{ \
		typedef NAME type; \
		typedef dictionary_encoding<NAME> encoding; \
		\
		template <typename Tree> \
		static void decode(\
				const std::string &prop, \
				const Tree &tr, \
				type &val \
				) \
		{ \
			if (prop.empty()) return; \
			BOOST_PP_SEQ_FOR_EACH_R( \
					1, \
					_GULACHEK_DECODE_PROP, \
					_, \
					ATTR_SEQ) \
		} \
		\
		template <typename Tree> \
		static void encode(\
				const type &val, \
				Tree &tr \
				) \
		{ \
			tr.child_count(BOOST_PP_SEQ_SIZE(ATTR_SEQ)); \
			BOOST_PP_SEQ_FOR_EACH_I( \
					_GULACHEK_ENCODE_PROP, \
					_, \
					ATTR_SEQ) \
		} \
	}; \
}

#define _GULACHEK_SEQTUPLE_TO_SEQ(SEQTUP) \
	BOOST_PP_SEQ_TAIL(BOOST_PP_CAT(BOOST_FUSION_DEFINE_STRUCT_FILLER_0(0,0)SEQTUP,_END))

#define GULACHEK_GTREE_DEFINE_DICT(NAMESPACE_SEQ, NAME, ATTRIBUTES)           \
    _GULACHEK_DEFINE_PMAP_IMPL(                                            		\
        (0)NAMESPACE_SEQ,                                                       \
        NAME,                                                                   \
        BOOST_PP_CAT(BOOST_FUSION_DEFINE_STRUCT_FILLER_0(0,0)ATTRIBUTES,_END),  \
        2)                                                                      \
				\
    _GULACHEK_PMAP_ADAPT_STRUCT(                                                  \
        BOOST_FUSION_ADAPT_STRUCT_NAMESPACE_DECLARATION((0)NAMESPACE_SEQ) NAME, \
				_GULACHEK_SEQTUPLE_TO_SEQ(ATTRIBUTES) \
				)


#endif
