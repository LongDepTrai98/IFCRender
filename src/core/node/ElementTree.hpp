#ifndef _INODE_PROPERTIES_HPP_
#define _INODE_PROPERTIES_HPP_
#include <memory>
#include <string>
namespace dragon
{
	class TreeNode
	{
	public: 
		virtual ~TreeNode() = default; 
		virtual std::string getLabelNode() = 0; 
		virtual int& getData() = 0; 
	public: 
		std::vector<std::shared_ptr<TreeNode>> children{}; 
	};
	class ElementTree
	{
	public: 
		virtual ~ElementTree() = default;
	public: 
		std::shared_ptr<TreeNode> m_Parent{ nullptr }; 
	};
}
#endif // !_INODE_PROPERTIES_HPP_
