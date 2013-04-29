#ifndef _TUPLE_H
#define _TUPLE_H

namespace bbk
{
template <typename T, unsigned dimension>
class Tuple
{
public:
	/// const element access
	inline const T& operator[](unsigned index) const {return elements_[index];}
	/// Element access
	inline       T& operator[](unsigned index)       {return elements_[index];}

	/// assignment
	inline Tuple& operator=(const Tuple &rhs) {
		for (size_t i = 0; i < dimension; ++i)
			elements_[i] = rhs.elements_[i];
		return *this;}

private:
	T elements_[dimension];
}; // class Tuple

} // namespace bbk
#endif /* _TUPLE_H */
