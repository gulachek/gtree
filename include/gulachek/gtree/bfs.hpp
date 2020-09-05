#ifndef GULACHEK_GTREE_BFS_HPP
#define GULACHEK_GTREE_BFS_HPP

#include "gulachek/gtree/block.hpp"

#include <queue>
#include <utility>
#include <functional>

namespace gulachek::gtree
{
	class bfs
	{
		public:
			typedef void Visitor(const block &b, std::size_t);

			template <typename Tree>
			void visit(
					const Tree &t, 
					const std::function<Visitor> &f
					)
			{
				_q.push(_closure(t, f));
				_visit();
			}

		private:
			typedef void Work(void);

			void _visit()
			{
				if (_q.empty()) return;
				auto f = std::move(_q.front()); _q.pop();
				f();
			}

			template <typename Tree>
			std::function<Work> _closure(
					const Tree &t, 
					const std::function<Visitor> &f
					)
			{
				return [=](){
					f(t.value(), t.child_count());
					for (std::size_t i = 0; i < t.child_count(); i++)
						_q.push(_closure(t.child(i), f));
					_visit();
				};
			}

			std::queue<std::function<Work>> _q;
	};
}

#endif
