namespace xui
{
	struct rectangle
	{
		int x, y, width, height;
	};

	enum class alignment
	{
		left    = 0,
		top     = 0,
		stretch = 1,
		right   = 2,
		bottom  = 2,
	};

	struct widget
	{
		std::vector<entity_id> children;
		rectangle bounds;
		alignment vertical_align;
		alignment horizontal_align;
		std::set<std::string> classes; //< css styling
	};

	namespace layouting
	{
		//! declares the layout "stacking"
		enum class stack_layout
		{
			horizontal,
			vertical
		};

		//! declares the layout "docking"
		struct dock_layout { };

		//! for defining the dock position in the parent
		enum class dock_position
		{
			left, right, top, bottom
		};

		//! declares the layout "flowing"
		enum class flow_layout
		{
			left_to_right,
			top_to_bottom,
			bottom_to_top,
			right_to_left
		};

		//! defines a size in "auto", "n pixels", "n percent"
		struct table_size
		{
			enum
			{
				automatic,
				absolute,
				relative,
			} scale;
			double size;
		};

		//! defines table layout with n rows and m columns
		struct table_layout
		{
			std::vector<table_size> rows, columns;
		};

		//! defines the coordinate for a widget in a table
		struct table_cell
		{
			int row, column;
		};

		//! defines a canvas layout
		struct canvas_layout { };

		struct canvas_coordinate
		{
			double absolute;
			double relative;
		};

		struct canvas_position
		{
			canvas_coordinate x, y;
		};
	}

	namespace controls
	{
		struct button
		{

		};

		struct check_box
		{
			enum check_state { unchecked, intermediate, checked };
			check_state state;
		};

		struct radio_button
		{
			int group;
		};

		struct label
		{
			std::string text;
		};

		struct text_box
		{
			std::string text;
		};

		struct text_area
		{
			std::string text;
		};

		struct picture
		{

		};

		struct panel
		{

		};
	}
}
