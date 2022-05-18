typedef struct v2i {
	i32 x, y;
} v2i;

typedef struct v4 {
	f32 r, g, b, a;
} v4;

typedef struct Rect2i {
	v2i topleft;
	v2i dim;
} Rect2i;

function v2i
v2Add(v2i val1, v2i val2) {
	v2i result;
	result.x = val1.x + val2.x;
	result.y = val1.y + val2.y;
	return result;
}

function v2i
getRectBottomright(Rect2i rect) {
	v2i result = v2Add(rect.topleft, rect.dim);
	return result;
}

function u32
colorToU32ARGB(v4 col) {
	u32 result = 
		((u32)(col.a * 255)) << 24 |
		((u32)(col.r * 255)) << 16 |
		((u32)(col.g * 255)) << 8 |
		((u32)(col.b * 255));
	return result;
}
