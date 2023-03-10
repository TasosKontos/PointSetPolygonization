#include "convex_hull.h"

vector<Point_2> ConvexHullAlg(vector<Point_2> pointSet, int edgeSelectionMethod) {
    vector<Point_2> polygon;
    pointSet = sortPointset(pointSet,"1b");
    polygon = createConvexHull(pointSet);
    removeUsedPoints(polygon,pointSet);
    addUntrackedCollinearPointsInPolygonalLine(polygon,pointSet);
    removeUsedPoints(polygon,pointSet);
    while(!pointSet.empty()) {
        vector<pair<Segment_2, Point_2>> pairOfSegmentAndClosestPoint = getPairOfClosestPointToSegments(pointSet,polygon);
        pair<Segment_2, Point_2> pairToReplace;
        switch (edgeSelectionMethod) {
            case 1:
                pairToReplace = randomPairSelection(pairOfSegmentAndClosestPoint, polygon);
                break;
            case 2:
                pairToReplace = minAreaPairSelection(pairOfSegmentAndClosestPoint, polygon);
                break;
            case 3:
                pairToReplace = maxAreaPairSelection(pairOfSegmentAndClosestPoint, polygon);
                break;
            default:
                cout << "Wrong edge selection method" << endl;
                exit(-1);
        }
        polygon = insertPointToPolygonPointSet(pairToReplace.second, pairToReplace.first, polygon);
        pointSet.erase(std::remove(pointSet.begin(), pointSet.end(), pairToReplace.second), pointSet.end());

    }
    return polygon;
}

void removeUsedPoints(vector<Point_2> polygon, vector<Point_2>& pointSet) {
    for(Point_2 & polygonPoint : polygon){
        pointSet.erase(std::remove(pointSet.begin(), pointSet.end(), polygonPoint), pointSet.end());
    }
}

void addUntrackedCollinearPointsInPolygonalLine(vector<Point_2>& polygon, vector<Point_2>& pointSet) {
    int size = pointSet.size()-1;
    for(int pointSetIndex=size ; pointSetIndex>=0 ; --pointSetIndex) {
        for(int polygonIndex=0 ; polygonIndex<polygon.size() ; ++polygonIndex) {
            int nextElementIndex = polygonIndex==(polygon.size()-1) ? 0 : (polygonIndex+1);
            const auto doIntersect = CGAL::intersection(Segment_2(polygon[polygonIndex],polygon[nextElementIndex]),pointSet[pointSetIndex]);
            if (doIntersect) {
                polygon = insertPointToPolygonPointSet(pointSet[pointSetIndex],
                                                       Segment_2(polygon[polygonIndex], polygon[nextElementIndex]),
                                                       polygon);
                break;
            }

        }
    }
}

vector<pair<Segment_2,Point_2>> getPairOfClosestPointToSegments(vector<Point_2> internalPoints, vector<Point_2> polygon) {
    vector<pair<Segment_2,Point_2>> pairs;
    vector<Segment_2> polygonEdges = getPolygonEdgesFromPoints(polygon);
    for(Segment_2 edge : polygonEdges) {
        Point_2 closestPoint = findClosestPointToSegment(edge,internalPoints);
        pairs.push_back(make_pair(edge,closestPoint));
    }
    return pairs;
};

Point_2 findClosestPointToSegment(Segment_2 seg, vector<Point_2> internalPoints){
    pair<Point_2, long double> minDistance(internalPoints[0], abs(squared_distance(internalPoints[0], seg)));
    for(int i=1; i<internalPoints.size(); i++){
        long double dist = abs(squared_distance(internalPoints[i], seg));
        if(dist<minDistance.second){
            minDistance.first = internalPoints[i];
            minDistance.second = dist;
        }
    }
    return minDistance.first;
}

pair<Segment_2,Point_2> randomPairSelection(vector<pair<Segment_2,Point_2>> pair, vector<Point_2> polygon) {
    vector<int> randomInts = createVectorOfRandomInts(pair.size());
    vector<Segment_2> polygonEdges = getPolygonEdgesFromPoints(polygon);
    for(int i=0 ; i<pair.size() ; ++i) {
        if( isEdgeVisibleFromPoint( pair[randomInts[i]].second, pair[randomInts[i]].first, polygonEdges) )
            return pair[randomInts[i]];
    }
    cout << "Couldn't find a visible point!" << endl << "Aborting..." << endl;
    exit(-1);
}

pair<Segment_2,Point_2> minAreaPairSelection(vector<pair<Segment_2, Point_2>> pairs, vector<Point_2> polygon) {
    vector<pair<pair<Segment_2, Point_2>,int>> results;
    for(pair<Segment_2, Point_2> pair : pairs){
        if(isEdgeVisibleFromPoint(pair.second, pair.first, getPolygonEdgesFromPoints(polygon))) {
            vector<Point_2> tempPolygon = insertPointToPolygonPointSet(pair.second, pair.first, polygon);
            results.push_back(make_pair(pair, abs(getSimplePolygonFromPoints(tempPolygon).area())));
        }
    }
    if(results.empty()) {
        cout << "Couldn't find a visible point!" << endl << "Aborting..." << endl;
        exit(-1);
    }
    pair<pair<Segment_2, Point_2>, int> minAreaPair = results[0];
    for(int i=1; i<=(results.size()-1); ++i){
        if(results[i].second<minAreaPair.second)
            minAreaPair = results[i];
    }
    return minAreaPair.first;
}

pair<Segment_2,Point_2> maxAreaPairSelection(vector<pair<Segment_2, Point_2>> pairs, vector<Point_2> polygon) {
    vector<pair<pair<Segment_2, Point_2>,int>> results;
    for(pair<Segment_2, Point_2> pair : pairs){
        if(isEdgeVisibleFromPoint(pair.second, pair.first, getPolygonEdgesFromPoints(polygon))) {
            vector<Point_2> tempPolygon = insertPointToPolygonPointSet(pair.second, pair.first, polygon);
            results.push_back(make_pair(pair, abs(getSimplePolygonFromPoints(tempPolygon).area())));
        }
    }
    if(results.empty()) {
        cout << "Couldn't find a visible point!" << endl << "Aborting..." << endl;
        exit(-1);
    }
    pair<pair<Segment_2, Point_2>, int> maxAreaPair = results[0];
    for(int i=1; i<=(results.size()-1); ++i){
        if(results[i].second>maxAreaPair.second)
            maxAreaPair = results[i];
    }
    return maxAreaPair.first;
}