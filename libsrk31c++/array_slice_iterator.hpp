	typedef std::pair< 	std::pair<const char *, Dwarf_Half>,
				std::pair<Dwarf_Half, const char *> > tag_lookup_pairs_t;

	// FIXME: this iterator class seems to be useful. Make it safer and more general.
	template<typename Target, typename Step> class array_step_iterator//<Target, Step>
	{
		Target *value;
	public:
		array_step_iterator(Target *value)
			: value(value) {}
		Target operator*() { return *value; }
		bool operator == (array_step_iterator<Target, Step> arg) { return arg.value == value; }
		bool operator != (array_step_iterator<Target, Step> arg) { return !(arg == *this); }
		array_step_iterator operator++() { value = reinterpret_cast<Target *>(
			reinterpret_cast<char *>(value) + sizeof (Step)); return *this; }
	};
	
	template<typename TargetType, typename ArrayType, size_t arrayLimit/*, ArrayType base[arrayLimit]*/> class array_slice_iterator
	{
		TargetType *value;
		ArrayType (&m_array_ref)[arrayLimit];
		ArrayType *m_base;
		size_t begin_offset;
		size_t stride_bytes;
		void *array_end;
	public:
		array_slice_iterator(ArrayType (&base)[arrayLimit], TargetType *first_value)
			: 	value(first_value), m_array_ref(base), m_base(&base[0]), 
				begin_offset(reinterpret_cast<char*>(first_value) - reinterpret_cast<char*>(m_base)),
				stride_bytes(sizeof(ArrayType)),
				array_end(static_cast<void*>(reinterpret_cast<char*>(m_base) + (arrayLimit * sizeof(ArrayType))))
		{
			// quick safety-check: is the first element in bounds?
			std::cerr 	<< "Array base address: " << &base[0] << std::endl
						<< "number of elements in array: " << arrayLimit << std::endl
						<< "Begin value offset: " << begin_offset << std::endl
						<< "Stride (bytes): " << stride_bytes << std::endl
						<< "Computed array end: " << array_end << std::endl;
			if (reinterpret_cast<char*>(value) > reinterpret_cast<char*>(array_end) + sizeof (ArrayType))
			{
				// we allow pointers only up to one sizeof(TargetType) off the end of the array
				// -- this is so we can support end()-style iterators, but otherwise raise
				// an out-of-range exception
				throw std::out_of_range(std::string("array_slice_iterator"));
			}
		}
		array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/> limit()
		{
			size_t elements_remaining_after_current = 
				(reinterpret_cast<char*>(array_end) - reinterpret_cast<char*>(value)) 
					/ sizeof (ArrayType);
			return array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/>(m_array_ref,
				reinterpret_cast<TargetType*>(reinterpret_cast<char*>(value) 
					+ sizeof(ArrayType) * (1 + elements_remaining_after_current))
				);
		}
		TargetType operator*() 
		{ 
			if (value < array_end && value >= static_cast<void*>(m_base))
			{ 
				return *value;
			}
			else throw std::out_of_range("array_slice_iterator"); 
		}
		bool operator == (array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/> arg) 
		{ return arg.value == value; }
		bool operator != (array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/>  arg) 
		{ return !(arg == *this); }
		array_slice_iterator operator-(unsigned reduction)
		{	
			return array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/>(m_array_ref,
				reinterpret_cast<TargetType*>(reinterpret_cast<char*>(value) - sizeof(ArrayType)));
		}
		array_slice_iterator operator++() { value = reinterpret_cast<TargetType *>(
			reinterpret_cast<char *>(value) + sizeof (ArrayType)); 
			if (value <= limit().value) return *this; else throw std::out_of_range(std::string("array_slice_iterator")); }
	};
	
	template<typename TargetType, typename ArrayType, 
		size_t arrayLimit/*, ArrayType base[arrayLimit]*/>
		array_slice_iterator<TargetType, ArrayType, arrayLimit/*, base*/>
		make_array_slice_iterator(ArrayType (&array)[arrayLimit], TargetType *element)

//make_array_slice_iterator(
//dwarf::tag_lookup_pairs_t [58], 
//std::pair<const char*, short unsigned int>*)

	{
 		return array_slice_iterator<TargetType, ArrayType, arrayLimit
 			/*sizeof array / sizeof(ArrayType)/*, array*/>
 			(array, element);
	}

#define PAIR_ENTRY(sym) std::make_pair( std::make_pair(#sym, (sym)), std::make_pair((sym), #sym))

	tag_lookup_pairs_t tag_lookup_pairs[] = { 
							PAIR_ENTRY(DW_TAG_array_type), 
							PAIR_ENTRY(DW_TAG_class_type), 
							PAIR_ENTRY(DW_TAG_entry_point), 
							PAIR_ENTRY(DW_TAG_enumeration_type), 
							PAIR_ENTRY(DW_TAG_formal_parameter),
							PAIR_ENTRY(DW_TAG_imported_declaration), 
							PAIR_ENTRY(DW_TAG_label), 
							PAIR_ENTRY(DW_TAG_lexical_block), 
							PAIR_ENTRY(DW_TAG_member), 
							PAIR_ENTRY(DW_TAG_pointer_type), 
							PAIR_ENTRY(DW_TAG_reference_type), 
							PAIR_ENTRY(DW_TAG_compile_unit), 
							PAIR_ENTRY(DW_TAG_string_type), 
							PAIR_ENTRY(DW_TAG_structure_type), 
							PAIR_ENTRY(DW_TAG_subroutine_type), 
							PAIR_ENTRY(DW_TAG_typedef), 
							PAIR_ENTRY(DW_TAG_union_type), 
							PAIR_ENTRY(DW_TAG_unspecified_parameters), 
							PAIR_ENTRY(DW_TAG_variant), 
							PAIR_ENTRY(DW_TAG_common_block), 
							PAIR_ENTRY(DW_TAG_common_inclusion), 
							PAIR_ENTRY(DW_TAG_inheritance), 
							PAIR_ENTRY(DW_TAG_inlined_subroutine), 
							PAIR_ENTRY(DW_TAG_module), 
							PAIR_ENTRY(DW_TAG_ptr_to_member_type), 
							PAIR_ENTRY(DW_TAG_set_type), 
							PAIR_ENTRY(DW_TAG_subrange_type), 
							PAIR_ENTRY(DW_TAG_with_stmt), 
							PAIR_ENTRY(DW_TAG_access_declaration), 
							PAIR_ENTRY(DW_TAG_base_type), 
							PAIR_ENTRY(DW_TAG_catch_block), 
							PAIR_ENTRY(DW_TAG_const_type), 
							PAIR_ENTRY(DW_TAG_constant), 
							PAIR_ENTRY(DW_TAG_enumerator), 
							PAIR_ENTRY(DW_TAG_file_type), 
							PAIR_ENTRY(DW_TAG_friend), 
							PAIR_ENTRY(DW_TAG_namelist), 
							PAIR_ENTRY(DW_TAG_namelist_item), /* DWARF3/2 spelling */
							PAIR_ENTRY(DW_TAG_packed_type), 
							PAIR_ENTRY(DW_TAG_subprogram), 
							PAIR_ENTRY(DW_TAG_template_type_parameter), /* DWARF3/2 spelling*/
							PAIR_ENTRY(DW_TAG_template_value_parameter), /* DWARF3/2 spelling*/
							PAIR_ENTRY(DW_TAG_thrown_type), 
							PAIR_ENTRY(DW_TAG_try_block), 
							PAIR_ENTRY(DW_TAG_variant_part), 
							PAIR_ENTRY(DW_TAG_variable), 
							PAIR_ENTRY(DW_TAG_volatile_type), 
							PAIR_ENTRY(DW_TAG_dwarf_procedure), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_restrict_type), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_interface_type), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_namespace), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_imported_module), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_unspecified_type), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_partial_unit), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_imported_unit), /* DWARF3 */
							PAIR_ENTRY(DW_TAG_mutable_type), /* Withdrawn from DWARF3 by DWARF3f. */
							PAIR_ENTRY(DW_TAG_condition), /* DWARF3f */
							PAIR_ENTRY(DW_TAG_shared_type) // should be index 0x40 /* DWARF3f */
							};
							
	std::map<Dwarf_Half, const char *> tag_lookup_map(
		make_array_slice_iterator(tag_lookup_pairs, &(tag_lookup_pairs[0].second)),
// 		array_slice_iterator<std::pair<Dwarf_Half, const char *>, tag_lookup_pairs_t, 
// 			sizeof tag_lookup_pairs / sizeof (tag_lookup_pairs_t), tag_lookup_pairs>(
// 			&(tag_lookup_pairs[0].second)),
		make_array_slice_iterator(tag_lookup_pairs,&(tag_lookup_pairs[0].second)).limit() - 1
// 		array_slice_iterator<std::pair<Dwarf_Half, const char *>, tag_lookup_pairs_t, 
// 			sizeof tag_lookup_pairs / sizeof (tag_lookup_pairs_t), tag_lookup_pairs>(
// 			&(tag_lookup_pairs[0].second)).limit() - 1
		);
	
		
	void print_tag_lookup_map()
	{
		for (std::map<Dwarf_Half, const char *>::iterator iter = tag_lookup_map.begin();
			iter != tag_lookup_map.end();
			iter++)
		{
			std::cout << iter->first << " <-> " << iter->second << std::endl;
		}
	}	
	
	std::map<const char *, Dwarf_Half> tag_lookup_inverse_map(
		make_array_slice_iterator(tag_lookup_pairs, &(tag_lookup_pairs[0].first)),
		make_array_slice_iterator(tag_lookup_pairs, &(tag_lookup_pairs[0].first)).limit() - 1
//		array_step_iterator<std::pair<const char *, Dwarf_Half>, tag_lookup_pairs_t>(&(tag_lookup_pairs[0].first)),
// 		array_slice_iterator<std::pair<const char *, Dwarf_Half>, tag_lookup_pairs_t, 
// 			sizeof tag_lookup_pairs / sizeof (tag_lookup_pairs_t), tag_lookup_pairs>(
// 			&(tag_lookup_pairs[0].first)),
		//&(tag_lookup_pairs[0].first)),
//		array_step_iterator<std::pair<const char *, Dwarf_Half>, tag_lookup_pairs_t>(&(tag_lookup_pairs[
// 		array_slice_iterator<std::pair<const char *, Dwarf_Half>, tag_lookup_pairs_t, 
// 			sizeof tag_lookup_pairs / sizeof (tag_lookup_pairs_t), tag_lookup_pairs>(
// 			&(tag_lookup_pairs[0].first)).limit() - 1
		);
