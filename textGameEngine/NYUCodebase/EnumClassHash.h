#ifndef ENUM_CLASS_HASH_H
#define ENUM_CLASS_HASH_H

class EnumClassHash
{
public:
	EnumClassHash();
	~EnumClassHash();

	template <typename ENTITY_TYPE>
	size_t operator()(ENTITY_TYPE entityType) const{
		return static_cast<size_t>(entityType);
	}
};

#endif