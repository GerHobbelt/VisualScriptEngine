#ifndef NODECOLLECTION_HPP
#define NODECOLLECTION_HPP

#include "NE_NodeId.hpp"

#include <vector>
#include <unordered_set>

namespace NE
{

class NodeCollection
{
	SERIALIZABLE;

public:
	NodeCollection ();
	NodeCollection (const std::unordered_set<NodeId>& nodeIds);
	~NodeCollection ();

	bool				IsEmpty () const;
	bool				Contains (const NodeId& nodeId) const;
	size_t				Count () const;
	NodeId				GetLast () const;

	void				Enumerate (const std::function<bool (const NodeId&)>& processor) const;
	void				Insert (const NodeId& nodeId);
	void				Erase (const NodeId& nodeId);
	void				Clear ();

	bool				operator== (const NodeCollection& rhs) const;
	bool				operator!= (const NodeCollection& rhs) const;

	Stream::Status		Read (InputStream& inputStream);
	Stream::Status		Write (OutputStream& outputStream) const;

private:
	std::vector<NodeId>			nodes;
	std::unordered_set<NodeId>	nodeSet;
};

}

#endif