#include<opencv2/opencv.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<iostream>
using namespace std;
using namespace cv;

int main(){
	system("color 3F");
	Mat srcImage1 = imread("C:\\Users\\jqb\\Pictures\\jiangqb_blue.jpg");
	Mat srcImage2 = imread("C:\\Users\\jqb\\Pictures\\jiangqb_red.jpg");
	if (!srcImage1.data || !srcImage2.data){
		cout << "There are someting wrong with loading the pictures!\n";
		return false;
	}
	//ʹ��surf���Ӽ��ؼ���
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	vector<KeyPoint> keypoints_object, keypoints_sence;
	detector.detect(srcImage1, keypoints_object);
	detector.detect(srcImage2, keypoints_sence);
	//����������������������
	Mat descriptors_object, descriptors_sence;
	SurfDescriptorExtractor extractor;
	extractor.compute(srcImage1, keypoints_object, descriptors_object);
	extractor.compute(srcImage2, keypoints_sence, descriptors_sence);
	//ʹ��FLANN���ӽ�������ƥ��
	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_object, descriptors_sence, matches);
	//������ؼ���֮�����������С����
	double max_dist = 0, min_dist = 100;
	for (int i = 0; i < descriptors_object.rows; i++){
		double dist = matches[i].distance;
		if (dist < min_dist)
			min_dist = dist;
		if (dist>max_dist)
			max_dist = dist;
	}
	cout << ">Max dist �����룺" << max_dist << endl;
	cout << "Min dist ��С���룺" << min_dist << endl;

	//����ƥ�����С��3*min_dist�ĵ��
	vector<DMatch> good_matches;
	for (int i = 0; i < descriptors_object.rows; i++){
		if (matches[i].distance < 3 * min_dist)
			good_matches.push_back(matches[i]);
	}

	//����ƥ�䵽�Ĺؼ���
	Mat img_matches;
	drawMatches(srcImage1, keypoints_object, srcImage2, keypoints_sence, good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	vector<Point2f> obj;
	vector<Point2f> sence;
	//��ƥ��ɹ���ƥ����л�ȡ�ؼ���
	for (size_t i = 0; i < good_matches.size(); i++){
		obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
		sence.push_back(keypoints_sence[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, sence, CV_RANSAC);//����͸�ӱ任
	//�Ӵ����ͼƬ�л�ȡ�ǵ�
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(srcImage1.cols, 0);
	obj_corners[2] = cvPoint(srcImage1.cols, srcImage1.rows);
	obj_corners[3] = cvPoint(0, srcImage1.rows);
	vector<Point2f> sence_corner(4);

	//����͸�ӱ任
	perspectiveTransform(obj_corners, sence_corner, H);
	//���Ƴ��ǵ�֮���ֱ��
	line(img_matches, sence_corner[0] + Point2f(static_cast<float>(srcImage1.cols), 0), sence_corner[1] + Point2f(static_cast<float>(srcImage1.cols), 0), Scalar(255, 0, 123), 4);
	line(img_matches, sence_corner[1] + Point2f(static_cast<float>(srcImage1.cols), 0), sence_corner[2] + Point2f(static_cast<float>(srcImage1.cols), 0), Scalar(255, 0, 123), 4);
	line(img_matches, sence_corner[2] + Point2f(static_cast<float>(srcImage1.cols), 0), sence_corner[3] + Point2f(static_cast<float>(srcImage1.cols), 0), Scalar(255, 0, 123), 4);
	line(img_matches, sence_corner[3] + Point2f(static_cast<float>(srcImage1.cols), 0), sence_corner[0] + Point2f(static_cast<float>(srcImage1.cols), 0), Scalar(255, 0, 123), 4);

	//��ʾ���ս��
	imshow("��Ч��ͼ��", img_matches);
	waitKey(0);
}