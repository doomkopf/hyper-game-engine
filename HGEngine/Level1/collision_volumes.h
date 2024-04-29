enum cv_t // collision volume type
{
	CV_NONE = 0,
	CV_SPHERE = 1,
	CV_CYLINDER = 2,
	CV_BOX = 3
};

//
// collision volumes
//

struct cylinder_t
{
	float h, r;
};

struct box_t
{
	float right, up, dir;
};