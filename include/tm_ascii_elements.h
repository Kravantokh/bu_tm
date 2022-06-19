/** \brief A buffer to store a number of ASCII characters. Its contents are dynamically allocated.
 * \param x, y - dimensions of the buffer
 * \param contents - the contents of the buffer
*/
struct tm_char_buffer{
	int x, y;
	char* contents[];
};

typedef struct tm_char_buffer tm_char_buffer;

/** \brief Simulated default constructor for char buffers.*/
void tm_init_char_buffer(tm_char_buffer buff){
	buff.x = 0;
	buff.y = 0;
	contents = 0;
}

/** \brief A function to allocate a \link tm_char_buffer tm_char_buffer\endlink struct of the given dimensions.
*
* You may set its contents by using \link tm_set_charbuffer_content tm_set_charbuffer_content \endlink
*/
tm_char_buffer* tm_allocate_char_buffer(int x, int y){
	tm_char_buffer buff = {x, y, 0};
	buff.contents = (char*)malloc(x*y*(sizeof(char*)));
	return buff;
}

/** \brief A function used to set the contents of an ASCII buffer.
*
* Returns the length of the string if successfull, otherwise it will return 0 to show that an error has occured.
* \param string - the string that should become the new content.
* \param char_buffer
*/
int tm_set_charbuffer_content(char* string, tm_char_buffer* char_buffer){
	int len = strlen(string);
	/* If a longer string is given than the buffer nothing should be done*/
	if( len > char_buffer.x*char_buffer.y)
		return 0;

	strcpy(string, char_buffer.contents);
	return len;
}

/** \brif A function to deallocate*/
void tm_deallocate_char_buffer(tm_char_buffer* buff){

}
