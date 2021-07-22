#ifndef GULACHEK_GTREE_MUTABLE_TREE_HPP
#define GULACHEK_GTREE_MUTABLE_TREE_HPP

#include "gulachek/gtree/tree.hpp"

#include <vector>
#include <cstdint>
#include <memory>

namespace gulachek::gtree
{
	// Implements Tree and mutable_tree requirements
	class mutable_tree
	{
		typedef std::vector<std::uint8_t> value_type;

		public:
			mutable_tree() :
				_value{},
				_children{}
			{}

			mutable_tree(value_type &&value) :
				_value{std::make_shared<value_type>(std::move(value))},
				_children{}
			{}

			mutable_tree(const std::vector<std::uint8_t> &value) :
				_value{std::make_shared<value_type>(value)},
				_children{}
			{}

			mutable_tree(std::vector<mutable_tree> &&children) :
				_value{},
				_children{std::move(children)}
			{}

			mutable_tree(const std::vector<mutable_tree> &children) :
				_value{},
				_children{children}
			{}

			mutable_tree(
					std::vector<std::uint8_t> &&value,
					std::vector<mutable_tree> &&children
					) :
				_value{std::make_shared<value_type>(std::move(value))},
				_children{std::move(children)}
			{}

			mutable_tree(
					const std::vector<std::uint8_t> &value,
					std::vector<mutable_tree> &&children
					) :
				_value{std::make_shared<value_type>(value)},
				_children{std::move(children)}
			{}

			mutable_tree(
					std::vector<std::uint8_t> &&value,
					const std::vector<mutable_tree> &children
					) :
				_value{std::make_shared<value_type>(std::move(value))},
				_children{children}
			{}

			mutable_tree(
					const std::vector<std::uint8_t> &value,
					const std::vector<mutable_tree> &children
					) :
				_value{std::make_shared<value_type>(value)},
				_children{children}
			{}

			const block value() const
			{
				return block{
						_value ? _value->data() : nullptr,
						_value ? _value->size() : 0
				};
			}

			block value()
			{
				if (_value && _value.use_count() > 1)
					_value = std::make_shared<value_type>(*_value);

				return block{
					_value ? _value->data() : nullptr,
					_value ? _value->size() : 0
				};
			}

			void value(const block &b)
			{
				_value = std::make_shared<value_type>(
						std::vector<std::uint8_t>{b.data(), b.data() + b.size()}
						);
			}

			std::size_t child_count() const
			{ return _children.size(); }

			void child_count(std::size_t n)
			{ _children.resize(n); }

			const mutable_tree& child(std::size_t i) const
			{ return _children[i]; }

			mutable_tree& child(std::size_t i)
			{ return _children[i]; }

		private:
			std::shared_ptr<value_type> _value;
			std::vector<mutable_tree> _children;
	};

	/*
	template <typename T>
	class is_mutable_tree
	{
		static constexpr std::uint8_t u8 = 0;

		// Has a value method to mutate the value:
		// template <typename Iterator> <-- std::uint8_t iterator
		// void value(Iterator first, Iterator last);
		template <typename U>
		static constexpr bool has_value(...) { return false; }

		template <typename U,
			decltype(std::declval<U>().value(&u8, &u8))*WithPtr = nullptr
				// could add more examples, make this way more complex,
				// or just wait until concepts arrive :) #silverBullet
			>
		static constexpr bool has_value(void*) { return true; }

		// Has a child_count method to resize number of children:
		// void child_count(std::size_t);
		template <typename U>
		static constexpr bool has_child_count(...) { return false; }

		template <typename U,
			decltype(std::declval<U>().child_count(0LU))*Dummy = nullptr
			>
		static constexpr bool has_child_count(void*) { return true; }

		// Has a child method to access a mutable child
		// is_mutable_tree<T>& child(std::size_t i);
		template <typename U>
		static constexpr bool has_child(...) { return false; }

		template <typename U,
			std::enable_if_t<
				std::is_lvalue_reference_v<decltype(std::declval<U>().child(0LU))> &&
				!std::is_const_v<decltype(std::declval<U>().child(0LU))> &&
				std::is_same_v<U, std::remove_reference_t<decltype(std::declval<U>().child(0LU))>>
				,int> = 0
			>
		static constexpr bool has_child(void*) { return true; }

		template <typename U,
			std::enable_if_t<
				std::is_lvalue_reference_v<decltype(std::declval<U>().child(0LU))> &&
				!std::is_const_v<decltype(std::declval<U>().child(0LU))> &&
				is_mutable_tree<std::remove_reference_t<decltype(std::declval<U>().child(0LU))>>::value
				,int> = 0
			>
		static constexpr bool has_child(void*) { return true; }

		public:
			static constexpr bool value = is_tree<T>::value &&
				has_value<T>(nullptr) &&
				has_child_count<T>(nullptr) &&
				has_child<T>(nullptr);
	};
	*/

	template <typename T>
	struct mt_implements_child_
	{
		static constexpr bool value = false;
	};

	template <typename T, typename Self>
	concept MutableTreeImplementsChild_ =
		std::is_lvalue_reference_v<T> &&
		!std::is_const_v<T> &&
		(std::is_same_v<std::remove_reference_t<T>, Self> ||
		mt_implements_child_<std::remove_reference_t<T>>::value);

	template <typename T>
	concept MutableTree = Tree<T> &&
	requires(T tr, const block &b, std::size_t i)
	{
		{ tr.value(b) };
		{ tr.child_count(i) };
		{ tr.child(i) } -> MutableTreeImplementsChild_<T>;
	};

	template <MutableTree T>
	struct mt_implements_child_<T>
	{
		static constexpr bool value = true;
	};
}

#endif
