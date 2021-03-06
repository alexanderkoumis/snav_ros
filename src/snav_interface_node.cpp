/****************************************************************************
 *   Copyright (c) 2017 Michael Shomin. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name ATLFlight nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * In addition Supplemental Terms apply.  See the SUPPLEMENTAL file.  
 ****************************************************************************/
#include "snav_interface/snav_interface.hpp"

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "snav_interface");
  ros::NodeHandle nh;
  ros::NodeHandle private_nh("~");

  double loop_freq, slow_loop_freq;
  private_nh.param("loop_frequency", loop_freq, 100.0);
  private_nh.param("low_freq_data_rate", slow_loop_freq, 5.0);

  bool publish_est_data;
  private_nh.param("publish_est_data", publish_est_data, true);
  bool publish_sim_data;
  private_nh.param("publish_sim_data", publish_sim_data, true);

  bool broadcast_tf, publish_pose;
  bool broadcast_des_tf, publish_des_pose;
  bool broadcast_gps_tf;
  bool broadcast_sim_gt_tf, publish_sim_gt_pose;
  private_nh.param("broadcast_tf", broadcast_tf, true);
  private_nh.param("broadcast_des_tf", broadcast_des_tf, true);
  private_nh.param("broadcast_gps_tf", broadcast_gps_tf, true);
  private_nh.param("broadcast_sim_gt_tf", broadcast_sim_gt_tf, true);
  private_nh.param("publish_pose", publish_pose, true);
  private_nh.param("publish_des_pose", publish_des_pose, true);
  private_nh.param("publish_sim_gt_pose", publish_sim_gt_pose, true);

  SnavInterface sn_iface(nh, private_nh);
  
  ros::Timer timer = nh.createTimer(ros::Duration(1.0/slow_loop_freq),
                                    &SnavInterface::PublishLowFrequencyData, &sn_iface);
  ros::WallRate loop_ctrl(loop_freq);

  while(ros::ok())
  {
    ros::spinOnce();

    sn_iface.UpdateSnavData();
    sn_iface.UpdatePoseMessages();

    if (publish_est_data)
    {
      if (broadcast_des_tf)
        sn_iface.BroadcastDesiredTf();
      if (publish_des_pose)
        sn_iface.PublishDesiredPose();
      if (broadcast_tf)
      {
        sn_iface.BroadcastEstTf();
        sn_iface.BroadcastBaseLinkNoRotTf();
        sn_iface.BroadcastBaseLinkStabTf();
      }
      if (publish_pose)
        sn_iface.PublishEstPose();
      if (broadcast_gps_tf)
        sn_iface.BroadcastGpsEnuTf();
    }

    if (publish_sim_data)
    {
      if (broadcast_sim_gt_tf)
        sn_iface.BroadcastSimGtTf();
      if (publish_sim_gt_pose)
        sn_iface.PublishSimGtPose();
    }

    loop_ctrl.sleep();
  }

  return 0;
}
