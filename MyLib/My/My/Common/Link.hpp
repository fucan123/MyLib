#pragma once

#ifndef _LINK_HPP_
#define _LINK_HPP_

template <typename type>
struct NODE 
{
	type  value; // 值
	NODE* prev;  // 上一个指针
	NODE* next;  // 下一个指针
	int   index; // 索引
};

template <typename type>
class Link
{
public:
	Link() {}
	// 添加数据
	NODE<type>* Add(type value);
	// 移除数据[flag=1->delete value flag=2->delete[] value]
	bool Remove(type v, int flag=0);
	// 链表头
	NODE<type>* Head();
	// 重置到链表
	NODE<type>* Reset(int num=0);
	// 当前链表指针
	NODE<type>* Current();
	// 上一个链表指针
	NODE<type>* Prev();
	// 下一个链表指针
	NODE<type>* Next();
	// 保存当前
	NODE<type>* Store();
	// 恢复保存
	NODE<type>* ReStore();
	// 是否是链表头
	bool IsHead(NODE<type>* p=nullptr);
	// 释放内存[flag=1->delete value flag=2->delete[] value]
	void Release(int flag=0);
	// 当前总条数
	int Count();
private:
	// 总条数
	int m_count = 0;

	// 链表头
	NODE<type>* m_head = nullptr;

	// 当前链表指针
	NODE<type>* m_current = nullptr;

	// 最后一个链表指针
	NODE<type>* m_last = nullptr;

	// 保存的链表指针
	NODE<type>* m_store = nullptr;
protected:
public:

};

// 添加数据
template<typename type>
NODE<type>* Link<type>::Add(type value)
{
#if 1
	NODE<type>* p = new NODE<type>;
	p->value = value;
	p->prev = m_last;
	p->next = nullptr;
	p->index = m_count;

	if (!m_head) {
		m_head = p;
		m_current = m_head;
		m_last = m_head;
	}
	else {
		m_last->next = p;
		m_last = p;
	}
	m_count++;

	return p;
#else
	return nullptr;
#endif
}

// 移除数据
template<typename type>
inline bool Link<type>::Remove(type v, int flag)
{
	NODE<type>* eq = nullptr;
	for (NODE<type>* p = m_head; p; p = p->next) {
		if (p->value == v) {
			eq = p;
			break;
		}	
	}
	if (eq) {
		if (m_current == eq) // 当前指针等于要删除的指针
			m_current = eq->next;

		if (eq->prev) { // 不是第一个
			eq->prev->next = eq->next;
		}
		else { // 第一个
			m_head = eq->next;
		}

		if (eq->next) { // 有下一个
			eq->next->prev = eq->prev;
		}
		if (eq == m_last) { // 最后一个
			m_last = eq->prev;
		}

		if (flag == 1)
			delete (void*)eq->value;
		else if (flag == 2)
			delete[] (void*)eq->value;
		delete eq;

		m_count--;
		return true;
	}
	return false;
}

// 链表头
template<typename type>
inline NODE<type>* Link<type>::Head()
{
	return m_head;
}

// 重置到链表
template<typename type>
NODE<type>* Link<type>::Reset(int num)
{
	int i = 0;
	for (NODE<type>* p = m_head; p; p = p->next, i++) {
		if (i == num) {
			m_current = p;
			break;
		}
	}
	return m_current;
}

// 当前链表指针
template<typename type>
NODE<type>* Link<type>::Current()
{
	return m_current;
}

// 上一个链表指针
template<typename type>
inline NODE<type>* Link<type>::Prev()
{
	if (!m_current)
		return nullptr;

	m_current = m_current->prev;
	return m_current;
}

// 下一个链表指针
template<typename type>
NODE<type>* Link<type>::Next()
{
	if (!m_current)
		return nullptr;

	m_current = m_current->next;
	return m_current;
}

// 保存当前
template<typename type>
inline NODE<type>* Link<type>::Store()
{
	m_store = m_current;
	return m_current;
}

// 恢复保存
template<typename type>
inline NODE<type>* Link<type>::ReStore()
{
	m_current = m_store;
	return m_current;
}

// 是否是链表头
template<typename type>
inline bool Link<type>::IsHead(NODE<type>* p)
{
	if (p == nullptr)
		p = m_current;
	return p == m_head;
}

// 释放内存
template<typename type>
void Link<type>::Release(int flag)
{
	NODE<type>* p = m_head;
	while (p) {
		NODE<type>* next = p->next;
		if (flag == 1)
			delete (void*)p->value;
		else if (flag == 2)
			delete[] (void*)p->value;

		delete p;
		p = next;
	}
	m_count = 0;
}

// 当前总条数
template<typename type>
inline int Link<type>::Count()
{
	return m_count;
}

#endif // !