#ifndef GULACHEK_GTREE_ENCODING_OPTIMIZATION_TYPE
#define GULACHEK_GTREE_ENCODING_OPTIMIZATION_TYPE

namespace gulachek::gtree
{
	enum class optimization_type
	{
		empty = 0, // no value, no children
		value = 1, // uses_value, no children
		container = 2, // no value, uses_children
		hybrid = 3 // uses_value, uses_children
	};

	// not sure why you'd need 'empty' for encoding optimization but seems incomplete w/o
}

#endif
