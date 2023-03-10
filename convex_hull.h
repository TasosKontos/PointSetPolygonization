#ifndef POINTSETPOLYGONIZATION_CONVEX_HULL_H
#define POINTSETPOLYGONIZATION_CONVEX_HULL_H

#include "includes.h"
#include "cgalStructsFunctions.h"
#include "prints.h"

vector<Point_2> ConvexHullAlg(vector<Point_2>, int);
void removeUsedPoints(vector<Point_2>, vector<Point_2>&);
void addUntrackedCollinearPointsInPolygonalLine(vector<Point_2>&, vector<Point_2>&);
vector<pair<Segment_2,Point_2>> getPairOfClosestPointToSegments(vector<Point_2>, vector<Point_2>);
Point_2 findClosestPointToSegment(Segment_2 , vector<Point_2>);
pair<Segment_2,Point_2> randomPairSelection(vector<pair<Segment_2,Point_2>>, vector<Point_2>);
pair<Segment_2,Point_2> minAreaPairSelection(vector<pair<Segment_2,Point_2>>, vector<Point_2>);
pair<Segment_2,Point_2> maxAreaPairSelection(vector<pair<Segment_2,Point_2>>, vector<Point_2>);

#endif