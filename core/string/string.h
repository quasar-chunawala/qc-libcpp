#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <initializer_list>
#include <iterator>

#include <core/compat.h>

namespace dev {

	struct string_long {
		char *m_buffer_ptr; // 8 bytes
		size_t m_size;      // 8 bytes
		size_t m_capacity;  // 8 bytes
	};

	struct string_short {
		static constexpr size_t capacity{23};
		char m_buffer[capacity]; // 23 bytes
		unsigned char m_size;    // 1 byte
	};

	class string {
		using iterator = char *;
		using const_iterator = const char *;

	private:
		union {
			string_short s;
			string_long l;
		} m_data;

		static constexpr size_t string_short_mask = 0x01;
		static constexpr size_t growth_factor = 2;

		// --- SSO flag helpers ---
		bool is_short_string();
		bool is_short_string() const;
		bool is_long_string();
		bool is_long_string() const;

		// --- size helpers ---
		size_t get_short_size();
		size_t get_short_size() const;
		void set_short_size(size_t size);
		size_t get_long_size();
		size_t get_long_size() const;
		void set_long_size(size_t size);
		void set_size(size_t size);
		void set_sso_flag();
		void turn_off_sso_flag();

		// --- capacity helpers ---
		size_t get_long_capacity();
		size_t get_short_capacity();
		void set_long_capacity(size_t capacity);

		// --- allocation helpers ---
		char *allocate_helper(size_t new_capacity);
		void deallocate_helper(char *buffer_ptr);
		void construct_slow_path(const char *chars_array, size_t len);
		void construct_fast_path(const char *chars_array, size_t len);

		bool is_full();

	public:
		// --- constructors ---
		string();
		string(const char *chars_array);
		string(const string &other);
		string(string &&other);

		// construct from an arbitrary range [first, last)
		template <typename InputIt>
		string(InputIt first, InputIt last) : string() {
			for (auto it{first}; it != last; ++it)
				push_back(*it);
		}

		// construct from an initializer list
		string(std::initializer_list<char> list)
		    : string{list.begin(), list.end()} {}

		// construct count copies of character ch
		string(size_t count, char ch);

		// --- destructor ---
		~string();

		// --- swap ---
		friend void swap(string &lhs, string &rhs);
		void swap(string &other);

		// --- assignment ---
		string &operator=(const string &other);
		string &operator=(string &&other);

		// --- capacity ---
		size_t size();
		size_t size() const;
		size_t capacity();
		bool empty();
		void reserve(size_t new_capacity);

		// --- element access ---
		char *data();
		const char *data() const;
		char &operator[](size_t pos);
		const char &operator[](size_t pos) const;
		char &at(size_t pos);
		char &front();
		char &back();

		// --- iterators ---
		char *begin();
		const char *begin() const;
		char *end();
		const char *end() const;

		// --- modifiers ---
		void push_back(char ch);
		void pop_back();

		// insert range [first, last) before pos
		template <typename InputIt>
		void insert(const_iterator pos, InputIt first, InputIt last) {
			auto pos_ = const_cast<iterator>(pos);
			auto first_ = first;
			auto last_ = last;
			if (first < last) {
				size_t offset = std::distance(begin(), pos_);
				size_t n = std::distance(first, last);
				size_t num_elems_to_shift = std::distance(pos_, end());
				size_t remaining_capacity = capacity() - size();

				// handle self-referential insertion
				string temp;
				if (first_ >= begin() && first_ < end() &&
				    last_ > begin() && last_ <= end()) {
					for (auto it{first_}; it != last_; ++it)
						temp.push_back(*it);
					first_ = temp.begin();
					last_ = temp.end();
				}

				if (n > remaining_capacity) {
					size_t excess_capacity_reqd = std::max(
					    static_cast<int>(n - remaining_capacity), 0);
					reserve(capacity() + excess_capacity_reqd);
					pos_ = std::next(begin(), offset);
				}

				if (n < num_elems_to_shift) {
					std::copy(end() - n, end(), end());
					std::copy_backward(pos_, end() - n, end());
				} else {
					std::copy(pos_, end(), pos_ + n);
				}

				std::copy(first_, last_, pos_);
				set_size(size() + n);
				char *buffer_ptr = data();
				buffer_ptr[size()] = '\0';
			}
		}

		iterator erase(const_iterator first, const_iterator last);
	};

} // namespace dev
