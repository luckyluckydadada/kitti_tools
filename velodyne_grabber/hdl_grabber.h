/*
* Software License Agreement (BSD License)
*
*  Point Cloud Library (PCL) - www.pointclouds.org
*  Copyright (c) 2012 The MITRE Corporation
*
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the copyright holder(s) nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
*/

#ifndef PCL_IO_HDL_GRABBER_H_
#define PCL_IO_HDL_GRABBER_H_

#include <pcl/io/grabber.h>
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <boost/asio.hpp>
#include <string>
#include "synchronized_queue.hpp"

#define HAVE_PCAP 1

#define HDL_Grabber_toRadians(x) ((x) * M_PI / 180.0)

namespace pcl {
/** \brief Grabber for the Velodyne High-Definition-Laser (HDL)
* \author Keven Ring <keven@mitre.org>
* \ingroup io
*/
typedef enum {
	ENUM_PTDEV_HDL_64E=0,
	ENUM_PTDEV_VLP_16=1
} EnumPtDev;
class PCL_EXPORTS HDLGrabber : public Grabber {
public:
    /** \brief Signal used for a single sector
        *         Represents 1 corrected packet from the HDL Velodyne
        */
    typedef void (sig_cb_velodyne_hdl_scan_point_cloud_xyz)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZ> >&, float, float);
    /** \brief Signal used for a single sector
        *         Represents 1 corrected packet from the HDL Velodyne.  Each laser has a different RGB
        */
    typedef void (sig_cb_velodyne_hdl_scan_point_cloud_xyzrgb)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZRGBA> >&, float, float);
    /** \brief Signal used for a single sector
        *         Represents 1 corrected packet from the HDL Velodyne with the returned intensity.
        */
    typedef void (sig_cb_velodyne_hdl_scan_point_cloud_xyzi)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZI> >&, float startAngle,
        float);
    /** \brief Signal used for a 360 degree sweep
        *         Represents multiple corrected packets from the HDL Velodyne
        *         This signal is sent when the Velodyne passes angle "0"
        */
    typedef void (sig_cb_velodyne_hdl_sweep_point_cloud_xyz)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZ> >&);
    /** \brief Signal used for a 360 degree sweep
        *         Represents multiple corrected packets from the HDL Velodyne with the returned intensity
        *         This signal is sent when the Velodyne passes angle "0"
        */
    typedef void (sig_cb_velodyne_hdl_sweep_point_cloud_xyzi)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZI> >&);
    /** \brief Signal used for a 360 degree sweep
        *         Represents multiple corrected packets from the HDL Velodyne
        *         This signal is sent when the Velodyne passes angle "0".  Each laser has a different RGB
        */
    typedef void (sig_cb_velodyne_hdl_sweep_point_cloud_xyzrgb)(
    const boost::shared_ptr< const pcl::PointCloud< pcl::PointXYZRGBA> >&);

    /** \brief Constructor taking an optional path to an HDL corrections file.  The Grabber will listen on the default IP/port for data packets [192.168.3.255/2368]
        * \param[in] correctionsFile Path to a file which contains the correction parameters for the HDL.  This parameter is mandatory for the HDL-64, optional for the HDL-32
        * \param[in] pcapFile Path to a file which contains previously captured data packets.  This parameter is optional
        * \param[in] dev
        * \param[in] broken_file
        * \param[in] check_brokenframe
        * \param[in] check_timestamp
        * \param[in] check_zeroblock
        */
    HDLGrabber(const std::string& correctionsFile, const std::string& pcapFile, EnumPtDev dev,
        const std::string& broken_file, const bool check_brokenframe, const bool check_timestamp, const bool check_zeroblock,
        boost::mutex& file_mutex);

    /** \brief virtual Destructor inherited from the Grabber interface. It never throws. */
    virtual ~HDLGrabber() throw();

    /** \brief Starts processing the Velodyne packets, either from the network or PCAP file. */
    virtual void start();

    /** \brief Stops processing the Velodyne packets, either from the network or PCAP file */
    virtual void stop();

    /** \brief Obtains the name of this I/O Grabber
        *  \return The name of the grabber
        */
    virtual std::string getName() const;

    /** \brief Check if the grabber is still running.
        *  \return TRUE if the grabber is running, FALSE otherwise
        */
    virtual bool isRunning() const;

    /** \brief Returns the number of frames per second.
        */
    virtual float getFramesPerSecond() const;

    /** \brief Allows one to filter packets based on the SOURCE IP address and PORT
        *         This can be used, for instance, if multiple HDL LIDARs are on the same network
        */
    void filterPackets(const boost::asio::ip::address& ipAddress,
        const unsigned short port = 443);

    /** \brief Allows one to customize the colors used for each of the lasers.
        */
    void setLaserColorRGB(const pcl::RGB& color, unsigned int laserNumber);

    /** \brief Any returns from the HDL with a distance less than this are discarded.
        *         This value is in meters
        *         Default: 0.0
        */
    void setMinimumDistanceThreshold(float& minThreshold);

    /** \brief Any returns from the HDL with a distance greater than this are discarded.
        *         This value is in meters
        *         Default: 10000.0
        */
    void setMaximumDistanceThreshold(float& maxThreshold);

    /** \brief Returns the current minimum distance threshold, in meters
        */

    float getMinimumDistanceThreshold();

    /** \brief Returns the current maximum distance threshold, in meters
        */
    float getMaximumDistanceThreshold();

    void setFrameCountIgnore(int ignore){
        frameCountIgnore = ignore;
    }

    void set_stamp_end(uint64_t stamp){
        _stamp_end = stamp;
    }

    bool _is_split_frame_global;//全局拆帧开关
    bool _is_data_away;
    const bool _check_brokenframe;
    const bool _check_timestamp;
    const bool _check_zeroblock;
    boost::mutex& _file_mutex;

protected:
    static const int HDL_DATA_PORT = 2368;
    static const int HDL_NUM_ROT_ANGLES = 36001;
    static const int HDL_LASER_PER_FIRING = 32;
    static const int HDL_MAX_NUM_LASERS = 64;
    static const int HDL_FIRING_PER_PKT = 12;
    static const boost::asio::ip::address HDL_DEFAULT_NETWORK_ADDRESS;

    enum HDLBlock {
        BLOCK_0_TO_31 = 0xeeff,
        BLOCK_32_TO_63 = 0xddff
    };

#pragma pack(push, 1)
    typedef struct HDLLaserReturn {
        unsigned short distance;
        unsigned char intensity;
    } HDLLaserReturn;
#pragma pack(pop)

    struct HDLFiringData {
        unsigned short blockIdentifier;
        unsigned short rotationalPosition;
        HDLLaserReturn laserReturns[HDL_LASER_PER_FIRING];
    };

    struct HDLDataPacket {
        HDLFiringData firingData[HDL_FIRING_PER_PKT];
        unsigned int gpsTimestamp;
        unsigned char blank1;
        unsigned char blank2;
    };

    struct HDLLaserCorrection {
        double azimuthCorrection;
        double verticalCorrection;
        double distanceCorrection;
        double verticalOffsetCorrection;
        double horizontalOffsetCorrection;
        double sinVertCorrection;
        double cosVertCorrection;
        double sinVertOffsetCorrection;
        double cosVertOffsetCorrection;
    };

#pragma pack(push, 1)
    struct AttitudeData
    {
        char unusedData0[14];

        unsigned short gyro1;
        unsigned short temp1;
        unsigned short accel1x;
        unsigned short accel1y;

        unsigned short gyro2;
        unsigned short temp2;
        unsigned short accel2x;
        unsigned short accel2y;

        unsigned short gyro3;
        unsigned short temp3;
        unsigned short accel3x;
        unsigned short accel3y;

        char unusedData1[160];
    };

    struct GpsData
    {
        unsigned int	GpsTimestamp;
        unsigned int	nUnused;
        char			NMEA[72];
        char			unusedData[234];
    };


    struct PositionData
    {
        AttitudeData attData;
        GpsData gpsData;
    };
#pragma pack(pop)

private:
    static double* cos_lookup_table_;
    static double* sin_lookup_table_;
    std::queue< unsigned char*> hdl_data_;
    //pcl::SynchronizedQueue< unsigned char*> hdl_data_;
    boost::asio::ip::udp::endpoint udp_listener_endpoint_;
    boost::asio::ip::address source_address_filter_;
    unsigned short source_port_filter_;
    boost::asio::io_service hdl_read_socket_service_;
    boost::asio::ip::udp::socket* hdl_read_socket_;
    std::string pcap_file_name_;
    boost::thread* queue_consumer_thread_;
    boost::thread* hdl_read_packet_thread_;
    HDLLaserCorrection laser_corrections_[HDL_MAX_NUM_LASERS];
    bool terminate_read_packet_thread_;
    boost::shared_ptr< pcl::PointCloud< pcl::PointXYZ> > current_scan_xyz_,
        current_sweep_xyz_;
    boost::shared_ptr< pcl::PointCloud< pcl::PointXYZI> > current_scan_xyzi_,
        current_sweep_xyzi_;
    boost::shared_ptr< pcl::PointCloud< pcl::PointXYZRGBA> > current_scan_xyzrgb_,
        current_sweep_xyzrgb_;
    unsigned int last_azimuth_;
    unsigned int lastAz;
    time_t last_ms;
    time_t _last_time_stamp;
    time_t last_us;
    int last_hour;
    boost::signals2::signal< sig_cb_velodyne_hdl_sweep_point_cloud_xyz>* sweep_xyz_signal_;
    boost::signals2::signal< sig_cb_velodyne_hdl_sweep_point_cloud_xyzrgb>*
        sweep_xyzrgb_signal_;
    boost::signals2::signal< sig_cb_velodyne_hdl_sweep_point_cloud_xyzi>*
        sweep_xyzi_signal_;
    boost::signals2::signal< sig_cb_velodyne_hdl_scan_point_cloud_xyz>* scan_xyz_signal_;
    boost::signals2::signal< sig_cb_velodyne_hdl_scan_point_cloud_xyzrgb>*
        scan_xyzrgb_signal_;
    boost::signals2::signal< sig_cb_velodyne_hdl_scan_point_cloud_xyzi>* scan_xyzi_signal_;
    pcl::RGB laser_rgb_mapping_[HDL_MAX_NUM_LASERS];
    float min_distance_threshold_;
    float max_distance_threshold_;

    void processVelodynePackets();
    void processVelodynePackets2();
    void enqueueHDLPacket(const unsigned char* data, std::size_t bytesReceived);
    void initialize(const std::string& correctionsFile);
    void loadCorrectionsFile(const std::string& correctionsFile);
    void loadHDL32Corrections();
    void loadVLP16Corrections();
    void readPacketsFromSocket();
#ifdef HAVE_PCAP
    void read_packets_from_pcap();
#endif //#ifdef HAVE_PCAP
    void toPointClouds(HDLDataPacket* dataPacket);
    void fireCurrentSweep();
    void fireCurrentScan(const unsigned short startAngle, const unsigned short endAngle);
    void computeXYZI(pcl::PointXYZI& pointXYZI, int azimuth, HDLLaserReturn laserReturn,
        HDLLaserCorrection correction);
    bool isAddressUnspecified(const boost::asio::ip::address& ip_address);
    bool _is_data_ready;
    int frameCountIgnore;
    uint64_t _stamp_end;
    int currentFrameCount;
    bool _is_broken_frame;
    //jake 获取imu姿态
    Eigen::Matrix4f _mat_imu_frame;
	EnumPtDev _current_dev;
    std::string _broken_file;
};
}

#endif /* PCL_IO_HDL_GRABBER_H_ */
