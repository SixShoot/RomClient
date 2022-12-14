#include "base_pcheader.h"


#include <base\base_Trie.h>

#ifdef TRIE_SUPPORT_PRINTING
#include <base\base_String.h>

#endif


namespace base
{


// ***** Internal Utility Routines - - - - - - - - - - - - - - - - - - - - - -

// Bitmap helper functions

enum { BITSPERBYTE = 8 };

inline int bit_check(unsigned char *bmdata, int bitnum)
{
	return (int)(( *((bmdata) + ((bitnum)-1)/BITSPERBYTE) >>
	  ((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE) )&1);
}

inline void bit_set(unsigned char *bmdata, int bitnum)
{
	( *((bmdata) + ((bitnum)-1)/BITSPERBYTE)  |=  
	  (1<<((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE)) );
}

inline void bit_clear(unsigned char *bmdata, int bitnum)
{
	( *((bmdata) + ((bitnum)-1)/BITSPERBYTE)  &= 
	  ~(1<<((BITSPERBYTE-1) - ((bitnum)-1)%BITSPERBYTE)) );
}

/*
**  KeyCmp() -- compare keys
**
**  key1, len1  -- key number 1 (&length in bytes)
**  key2, len2  -- key number 2 (&length in bytes)
**
**  RETURNS:
**   <0 = (degenerate case) one or both of the keys has 0 length
**    0 = keys are the same
**   >0 = 1st bit number where keys differ
*/
static int KeyCmp(unsigned char *key1, int len1, unsigned char *key2, int len2)
{
	int ii, bitnum;
	int bv1, bv2;

	if(!len1 || !len2)
		return -1;

	for(ii = 0; ii < len1 && ii < len2; ++ii)
	{
		if(key1[ii] != key2[ii])
		{
			for(bitnum = ii*BITSPERBYTE+1; bitnum < (ii+1)*BITSPERBYTE; ++bitnum)
			{
				bv1 = bit_check(key1, bitnum);
				bv2 = bit_check(key2, bitnum);
				if((bv1 && !bv2) || (!bv1 && bv2))
					break;
			}
			return bitnum;
		}
	}

	if(len1 == len2)
	{
		return 0;
	}

	return ii*BITSPERBYTE+1;
}


// ***** Internal Trie Node class implementation - - - - - - - - - - - - - - -

class trie_node 
{
public:
	trie_node();
	trie_node(trie_node &node2);
	~trie_node();

	uint32_t  m_nextbit;	// decision bit for next node
	trie_node      *m_parent;	// NULL in trie root only
	trie_node      *m_child0;	// child trie for 0/1 bits respectively
	trie_node      *m_child1;	//   NOTE: if one is set both are set unless
	void           *m_data;		//         m_data is also set

	void *insert(uint32_t keydiff, unsigned char *key, uint32_t keylen, void *data, int keyTest);
};

trie_node::trie_node()
{
	m_nextbit = 0;
	m_parent  = NULL;
	m_child0  = NULL;
	m_child1  = NULL;
	m_data    = NULL;
}

trie_node::trie_node(trie_node &node2)
{
	m_nextbit = node2.m_nextbit;
	m_parent  = node2.m_parent;
	m_child0  = node2.m_child0;
	m_child1  = node2.m_child1;
	m_data    = node2.m_data;
}

trie_node::~trie_node()
{
	if(m_child0)
		delete(m_child0);
	if(m_child1)
		delete(m_child1);
}

/*
**  trie_node::Insert() -- used by trie_add() to actually add the key to the trie
*/
void *trie_node::insert(uint32_t keydiff, unsigned char *key, uint32_t keylen, void *data, int keyTest)
{
	trie_node *cur, *prev, *tmptn;

	cur = this;

	if(keydiff == 0)
		return NULL; // key already exists!

	else if(keydiff < cur->m_nextbit)
	{
		// locate correct insertion point
		for(prev = cur, cur = cur->m_parent;
		  cur != NULL && keydiff < cur->m_nextbit;
		  prev = cur, cur = cur->m_parent)
			;

		// add new decision node
		if((tmptn = new trie_node) == NULL)
			return NULL;

		if(cur == NULL) // insert before top of trie?
		{
			// copy trietop to new node
			*tmptn = *prev;

			// overwrite trietop with new data
			cur = prev;
			cur->m_data    = NULL;
			cur->m_nextbit = keydiff;
			if(bit_check(key, keydiff))
			{
				cur->m_child0 = tmptn;
				cur->m_child1 = NULL;
			}
			else
			{
				cur->m_child0 = NULL;
				cur->m_child1 = tmptn;
			}

			// link parent pointers to/from new node
			tmptn->m_parent = cur;
			if(tmptn->m_child0)
				tmptn->m_child0->m_parent = tmptn;
			if(tmptn->m_child1)
				tmptn->m_child1->m_parent = tmptn;
		}
		else
		{
			// insert node between trie & prev
			if(keyTest)
				tmptn->m_child1 = prev;
			else
				tmptn->m_child0 = prev;
			prev->m_parent = tmptn;

			if(cur->m_child0 == prev)
				cur->m_child0 = tmptn;
			else
				cur->m_child1 = tmptn;
			tmptn->m_parent = cur;

			cur = tmptn;
			cur->m_data    = NULL;
			cur->m_nextbit = keydiff;
		}
	}

	// need new child node?
	if(keylen*BITSPERBYTE+1 == keydiff)
	{
		cur->m_data = data;
	}
	else
	{
		if((tmptn = new trie_node) == NULL)
		{
			return NULL;
		}
		// set key in new leaf node
		tmptn->m_nextbit = keylen*BITSPERBYTE+1;
		tmptn->m_data    = data;

		if(bit_check(key, keydiff))
			cur->m_child1 = tmptn;
		else
			cur->m_child0 = tmptn;
		tmptn->m_parent = cur;
	}
	return data;
}

// ***** Advertised Functions  - - - - - - - - - - - - - - - - - - - - - - - -

CTrieIterator::CTrieIterator(CTrie &trie)
{
	m_trie = &trie;
	rewind();
}

void CTrieIterator::rewind()
{
	m_node = m_trie->m_root;

	// handle null tries
	if(m_node->m_child0 == NULL && m_node->m_child1 == NULL && m_node->m_data == NULL)
	{
		m_node = NULL;
		return;
	}

	// search down to left hand child (1st in order)
	if(m_node)
	{
		while(m_node->m_child0)
			m_node = m_node->m_child0;
	}
}

bool CTrieIterator::operator --(void)
{
	// at end? 
	if(!m_node)
	{
		m_node = m_trie->m_root;

		if(m_node)
		{
			while(m_node->m_child1)
				m_node = m_node->m_child1;
			return true;
		}
		else
			// trie is empty
			return false;
	}

	if(m_node->m_child0)
	{
		// there's more under this subtree, go left and
		// then to the rightmost sub-subtree
		m_node = m_node->m_child0;
		while(m_node->m_child1)
		{
			m_node = m_node->m_child1;
		}
		return true;
	}

	// no more under this subtree, go up until we find something
	trie_node *oldnode;
	while(oldnode = m_node, m_node = m_node->m_parent)
	{
		if(oldnode == m_node->m_child1)
		{
			// data at current level?
			if(m_node->m_data)
				return true;
			// data under left subtree?
			else if(m_node->m_child0)
			{
				// there's more under this subtree, go left and
				// then to the rightmost sub-subtree
				m_node = m_node->m_child0;
				while(m_node->m_child1)
				{
					m_node = m_node->m_child1;
				}
				return true;
			}
		}
		// no, go up again
	}

	return false;
}

bool CTrieIterator::operator ++(void)
{
	// already at end? 
	if(!m_node)
	{
		return false;
	}

	if(m_node->m_child1)
	{
		// there's more under this subtree, go right and
		// then to the leftmost sub-subtree
		m_node = m_node->m_child1;
		while(m_node->m_child0)
		{
			m_node = m_node->m_child0;
		}
		return true;
	}

	// no more under this subtree, go up
	trie_node *oldnode;
	while(oldnode = m_node, m_node = m_node->m_parent)
	{
		if(m_node->m_child0 == oldnode)
		{
			// data at current level?
			if(m_node->m_data)
				return true;
			// data under right subtree?
			else if(m_node->m_child1)
			{
				// there's more under this subtree, go right and
				// then to the leftmost sub-subtree
				m_node = m_node->m_child1;
				while(m_node->m_child0)
				{
					m_node = m_node->m_child0;
				}
				return true;
			}
		}
		// no, go up again
	}

	return false;
}

void *CTrieIterator::getData(void) const
{
	if(!m_node)
		return NULL;
	return m_node->m_data;
}

bool CTrieIterator::atBegin(void) const
{
	// scan to find first node
	trie_node *pnode = m_trie->m_root;
	if(!pnode)
		return true;	// REALLY empty trie (should not happen)
	while(pnode->m_child0)
		pnode = pnode->m_child0;

	return pnode == m_node;
}
bool CTrieIterator::atEnd(void) const
{
	return !m_node;
}


CTrie::CTrie()
{
	m_root = new trie_node;
}

CTrie::~CTrie(void)
{
	delete m_root;
}

/*
**  CTrie::Find() -- perform key lookup in trie
**
**  key     - key to search for
**  keylen  - length of key in bytes
**
**  RETURNS: pointer to user data (NULL if key not in trie)
**
**  PROCESS:
**    set current node to top of trie
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                return user data at current node
**            if bit difference < node bit number
**                return key not found
**        if keylen < next bit
**                return key not found
**        compare next bit & switch to correct subtree
**    return key not found
*/
void *CTrie::find(unsigned char *key, uint32_t keylen)
{
	unsigned char *curkey;
	uint32_t curkeylen;
	uint32_t keydiff;
	trie_node *cur;

	cur = this->m_root;
	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			getKey(cur->m_data, &curkey, &curkeylen);

			if(keylen == curkeylen)
			{
				keydiff = KeyCmp(key, keylen, curkey, curkeylen);

				if(keydiff == 0)
					return cur->m_data;
				else if(keydiff < cur->m_nextbit)
					return NULL;
			}
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
			return NULL;

		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	return NULL;
}

/*
**  CTrie::Add() -- add key to trie
**
**  Add a key to the trie.  If the key is already in the trie, the add will
**  fail with a NULL return (it will also do so if a new node could not be
**  created).
**
**  key     - key to add
**  keylen  - length of key in bytes
**  data    - data for key
**
**  RETURNS: pointer to user data (NULL if add failed)
**
**  PROCESS:
**    set current node to top of trie
**    if trie is empty (next bit is 0)
**        set key at current level & return success
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                return add failure (already exists)
**            if bit difference < node bit number
**                back up trie until difference > bit number
**                insert new node between current, prev node
**                add child node to new node for user key & return success
**        if keylen < next bit
**            scan down link0 until a key is found
**            get bit difference to current node
**            back up trie until difference > bit number
**            insert new node between current, prev node
**            add child node to new node for user key & return success
**        compare next bit & switch to correct subtree
**    add child node to prev node for user key & return success
*/
//void *CTrie::Add(unsigned char *key, int keylen, void *data)
void *CTrie::add(void *data)
{
	trie_node *prev, *cur;
	unsigned char *curkey;
	uint32_t curkeylen;
	uint32_t keydiff;

	if((cur = this->m_root) == NULL)
		return NULL;

	unsigned char *key;
	uint32_t keylen;
	getKey(data, &key, &keylen);

	if(keylen <= 0)
		return NULL;	// bad key, cannot insert

	if(cur->m_nextbit == 0) // empty trie?
	{
		cur->m_nextbit = keylen*BITSPERBYTE+1;
		cur->m_data    = data;
		return data;
	}

	prev = NULL;
	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			getKey(cur->m_data, &curkey, &curkeylen);

			if(keylen == curkeylen)
			{
				keydiff = KeyCmp(key, keylen, curkey, curkeylen);

				if(keydiff < cur->m_nextbit)
					return cur->insert(keydiff, key, keylen, data, bit_check(curkey, keydiff));
			}
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
		{
			for(; !cur->m_data; cur = cur->m_child0)
				;

			getKey(cur->m_data, &curkey, &curkeylen);
			keydiff = KeyCmp(key, keylen, curkey, curkeylen);

			return cur->insert(keydiff, key, keylen, data, bit_check(curkey, keydiff));
		}

		prev = cur;
		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	getKey(prev->m_data, &curkey, &curkeylen);
	keydiff = KeyCmp(key, keylen, curkey, curkeylen);

	return prev->insert(keydiff, key, keylen, data, bit_check(curkey, keydiff));
}

/*
**  CTrie::Del() -- remove key to trie
**
**  Remove a key from the trie.  If the key is not in the trie, the delete
**  will fail with a NULL return.
**
**  key     - key to remove
**  keylen  - length of key in bytes
**
**  RETURNS: pointer to user data (NULL if del failed)
**
**  PROCESS:
**    set current node to top of trie
**    while current node is not NULL
**        if a key exists at current level
**            get bit difference to current node
**            if no difference
**                point parent node's parent to sibling node
**                free current, parent nodes
**                return success
**        if keylen < next bit
**                return key not found
**        compare next bit & switch to correct subtree
**    return key not found
*/
void *CTrie::del(void *data)
{
	unsigned char *key;
	uint32_t keylen;

	getKey(data, &key, &keylen);
	return del(key, keylen);
}
void *CTrie::del(unsigned char *key, uint32_t keylen)
{
	trie_node *prev, *child, *cur;
	unsigned char *curkey;
	uint32_t curkeylen;
	uint32_t keydiff;
	void *data;

	cur = this->m_root;

	while(cur != NULL)
	{
		if(cur->m_data != NULL)
		{
			getKey(cur->m_data, &curkey, &curkeylen);

			if(keylen == curkeylen)
			{
				keydiff = KeyCmp(key, keylen, curkey, curkeylen);

				if(keydiff == 0)
				{
					data = cur->m_data; // save for return
					cur->m_data = NULL;

					while(cur->m_data == NULL)
					{
						// fully populated interior node?
						if(cur->m_child0 && cur->m_child1)
							break;

						// 0 or 1 m_childX entries are !=0
						prev = cur->m_parent;
						if(cur->m_child0)
							child = cur->m_child0;
						else
							child = cur->m_child1;

						// top-level node?
						if(!prev)
						{
							// empty trie?
							if(!child)
							{
								cur->m_nextbit = 0;
								cur->m_child0  = NULL;
								cur->m_child1  = NULL;
								break;
							}

							// copy child to current
							*cur = *child;
							cur->m_parent = NULL;
							// point children to trie
							if(cur->m_child0)
								cur->m_child0->m_parent = cur;
							if(cur->m_child1)
								cur->m_child1->m_parent = cur;
							free(child);
							break;
						}

						// sparsely populated interior node
						if(prev->m_child0 == cur)
							prev->m_child0 = child;
						else
							prev->m_child1 = child;
						if(child)
							child->m_parent = prev;
						free(cur);
						cur = prev;
					}

					return data;
				}

				if(keydiff < cur->m_nextbit)
					return NULL;
			}
		}

		if(keylen < (cur->m_nextbit+BITSPERBYTE-1)/BITSPERBYTE)
			return NULL;

		if(bit_check(key, cur->m_nextbit))
			cur = cur->m_child1;
		else
			cur = cur->m_child0;
	}

	return NULL;
}

#ifdef TRIE_SUPPORT_PRINTING
void CTrie::printNode(const char *szPrefix, char chHeader, trie_node *pNode)
{
	if(pNode)
	{
		String str;
		printf("%s", szPrefix);						// header line
		printf("+---0x%08x:  %4d 0x%08x    0x%08x  0x%08x  0x%08x\n",
			pNode, pNode->m_nextbit, pNode->m_data,
			pNode->m_parent, pNode->m_child0, pNode->m_child1);

		str = szPrefix;
		str += chHeader;
		str += "   ";

		if(pNode->m_data)
		{
			char chSpacer = (pNode->m_child0 || pNode->m_child1) ? '|' : ' ';
			printKey((str + chSpacer + "   ").c_str(), pNode);	// data line
		}

		if(pNode->m_child0)
		{
			printf("%s%c\n", str.c_str(), '|'	);				// spacer line
			char chSpacer = pNode->m_child1 ? '|' : ' ';
			printNode(str.c_str(), chSpacer, pNode->m_child0);	// child node
		}
		if(pNode->m_child1)
		{
			printf("%s%c\n", str.c_str(), '|');					// spacer line
			printNode(str.c_str(), ' ', pNode->m_child1);		// child node
		}
	}
}

void CTrie::print(void)
{
	printNode("", ' ', this->m_root);
}
#endif
}