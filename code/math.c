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
v2iAdd(v2i val1, v2i val2) {
	v2i result;
	result.x = val1.x + val2.x;
	result.y = val1.y + val2.y;
	return result;
}

function v2i
v2iSub(v2i val1, v2i val2) {
	v2i result;
	result.x = val1.x - val2.x;
	result.y = val1.y - val2.y;
	return result;
}

function v2i
v2iMax(v2i val1, v2i val2) {
	v2i result;
	result.x = max(val1.x, val2.x);
	result.y = max(val1.y, val2.y);
	return result;
}

function v2i
v2iMin(v2i val1, v2i val2) {
	v2i result;
	result.x = min(val1.x, val2.x);
	result.y = min(val1.y, val2.y);
	return result;
}

function v2i
v2iDivScalar(v2i vec, i32 by) {
	v2i result;
	result.x = vec.x / by;
	result.y = vec.y / by;
	return result;
}

function Rect2i
rectCenterDim(v2i center, v2i dim) {
	Rect2i result;
	result.dim = dim;
	result.topleft = v2iSub(center, v2iDivScalar(dim, 2));
	return result;
}

function v2i
getRectBottomright(Rect2i rect) {
	v2i result = v2iAdd(rect.topleft, rect.dim);
	return result;
}

function Rect2i
clipRectToRect(Rect2i rect, Rect2i clip) {
	v2i resultTopleft = v2iMax(rect.topleft, clip.topleft);

	v2i rectBottomright = getRectBottomright(rect);
	v2i clipBottomright = getRectBottomright(clip);

	v2i resultBottomright = v2iMin(rectBottomright, clipBottomright);

	v2i resultDim = v2iSub(resultBottomright, resultTopleft);
	Rect2i result = {resultTopleft, resultDim};
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
