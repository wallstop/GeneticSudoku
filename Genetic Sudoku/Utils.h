#pragma once

/* A handy formula to index into a single dimensional array as if it were a 2D array with row-length size */
inline int convertCoordinates(int x, int y, int size)
{
	return (x + (y * size));
}