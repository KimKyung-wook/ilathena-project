<?
include "setup.php";

$mode = $_GET["mode"];
$adid = $_POST["admin_id"];
$adpw = $_POST["admin_pw"];
if(!$mode) $mode = "list";
if($adid && !eregi(getenv("HTTP_HOST"),getenv("HTTP_REFERER"))) {
	msgbox("�������� ������ �ƴմϴ�...");
}
unset($connect);
unset($member);
if($zboard_use) {
			
			include_once $_zb_path."lib.php";
			
			if(!$connect) $connect = dbconn();
			if(!$member) $member = member_info();
	
}
else if($lunaboard_use) {
			
			include $lunapath."luna_lib.php";
			
			if(!$connect) $connect = dbconn();
			if(!$member) $member = member_info();
			
}
$isadmin = Admins();

if(!$isadmin) {
		msgbox("�����ڸ� ���尡���մϴ�...");
}

$page = $_GET["page"];
if(!$page) $page = 1;
$search = $_GET["search"];
if(!$search) $search = $_POST["search"];
		
function msgbox($msg,$url="") {

	$msg = addslashes($msg);
	$msg = str_replace("<?","&lt;?",$msg);
	$msg = str_replace("?>","?&gt;",$msg);
	$msg = nl2br($msg);
	
	if($url) {
		if($url == "window.close()") $surl = $url;
		else $surl = "document.location='$url'";
	}
	else $surl = "history.go(-1)";
	
	echo "
			<table width='277' cellspacing='0' style='border-collapse:collapse;' align='center'>
    <tr>
        <td width='275' style='border-top-width:0; border-right-width:0; border-bottom-width:3; border-left-width:0; border-top-color:black; border-right-color:black; border-bottom-color:rgb(204,231,255); border-left-color:black; border-top-style:none; border-right-style:none; border-bottom-style:solid; border-left-style:none;' bgcolor='white'><span style='font-size:9pt;'><font color='#999999'>�޽���</font></span></td>
    </tr>
    <tr>
        <td width='275' style='border-top-width:1; border-right-width:0; border-bottom-width:1; border-left-width:0; border-top-color:rgb(153,153,153); border-right-color:black; border-bottom-color:rgb(153,153,153); border-left-color:black; border-top-style:dotted; border-right-style:none; border-bottom-style:dotted; border-left-style:none;' height='39'>
            <p align='center'><span style='font-size:9pt;'><font color='#333333'><br>$msg</font></span><br><br><input type='button' name='okbak' value='�ڷ�' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(165,180,224); border-style:solid;' onclick=\"javascript:$surl;\"></p>
</td>
    </tr>
</table>
";
exit;
}

function Admins() {
	
	global $admin_id,$admin_pw,$zboard_use,$lunaboard_use,$_zb_path,$lunapath,$adid,$adpw,$_SESSION,$_COOKIE,$member,$connect;
	
	if($zboard_use && $lunaboard_use) $zboard_use = 0;
	if(!$zboard_use && !$lunaboard_use && !$admin_id && !$admin_pw) return false;
	if($zboard_use && (!$_zb_path  || $_zb_path == "���")) return false;
	if($lunaboard_use && (!$lunapath  || $lunapath == "���")) return false;
	
	if($zboard_use) {
			
			if($member[is_admin] == 1 || $member[level] == 1) return true;
			else return false;
			
  }
	
  else if($lunaboard_use) {
			
			if($member[isadmin] == 1 || $member[level] == 1 || $member[sadmin] == 1) return true;
			else return false;
			
  }
  else {
  		if($adid && $adpw && $adid == $admin_id && $adpw == $admin_pw) {
  			@session_start();
  			$isadmin = $adpw;
  			@session_register("isadmin");
  		  	return true;
  		 }
  		else if($_SESSION["isadmin"] == $admin_pw) return true;
  		else if($adid && $adpw && ($adid != $admin_id || $adpw != $admin_pw)) return false;
  		else {
  		echo "
    <script>
    <!--
    function chkfrmadlogin(form) {
    
    	if(!form.admin_id.value) {
    		alert('������ ���̵� �Է��ϼ���..')
    		form.admin_id.focus();
    		return false;
    	}
    	if(!form.admin_pw.value) {
    		alert('������ ��й�ȣ�� �Է��ϼ���..')
    		form.admin_pw.focus();
    		return false;
    	}
    }
    //-->
    </script>
    		
    <form name='loginfrms' method=post action='$PHP_SELF?mode=list' onsubmit='return chkfrmadlogin(this);'>
<table width='315' cellspacing='0' style='border-collapse:collapse;' align='center'>
    <tr>
        <td width='313' style='border-top-width:0; border-right-width:0; border-bottom-width:3; border-left-width:0; border-top-color:black; border-right-color:black; border-bottom-color:rgb(204,231,255); border-left-color:black; border-top-style:none; border-right-style:none; border-bottom-style:solid; border-left-style:none;' bgcolor='white' colspan='3'><span style='font-size:9pt;'><font color='#666666'><b>������ �α���</b></font></span></td>
    </tr>
    <tr>
        <td width='313' style='border-top-width:1; border-right-width:0; border-bottom-width:1; border-left-width:0; border-top-color:rgb(153,153,153); border-right-color:black; border-bottom-color:rgb(153,153,153); border-left-color:black; border-top-style:dotted; border-right-style:none; border-bottom-style:dotted; border-left-style:none;' height='39' colspan='3'>

            <table width='311' cellspacing='0' style='border-collapse:collapse;'>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>���̵�</font></span></td>
                    <td width='223' style='border-width:0; border-color:black; border-style:none;'>

                            <p><span style='font-size:9pt;'><font color='#666666'><input type='text' name='admin_id' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(153,153,153); border-style:dotted;'></font></span></p>

</td>
                </tr>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>��й�ȣ</font></span></td>
                        <td width='223' style='border-width:0; border-color:black; border-style:none;'>                            <p><span style='font-size:9pt;'><font color='#666666'><input type='password' name='admin_pw' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(153,153,153); border-style:dotted;' size='22'></font></span></p>
</td>
                </tr>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>&nbsp;</font></span></td>
                        <td width='223' style='border-width:0; border-color:black; border-style:none;'>                            <p align='right'><span style='font-size:9pt;'><input type='submit' name='loginok' value='�α���' style='font-size:8pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(165,180,224); border-style:solid;'></span></p>
</td>
                </tr>
            </table>
</td>
    </tr>
</table>                        </form>
";
exit;
	}

 }
}
  		
function isblanks($str) {
	
	// ���ڿ��ΰ�� True ��ȯ ���ڿ��� �ƴѰ�� False �� ��ȯ�Ѵ�.
	$strs = str_replace("\n","",$str);
	$strs = str_replace("\r","",$strs);
	$strs = str_replace("&nbsp;","",$strs);
	$strs = str_replace(" ","",$strs);
	$strs = strip_tags($strs);
	$strs = trim($strs);
	if(eregi("[^[:space:]]",$strs)) return false;
	return true;
}

$skin_dir = "skin/".$skin;
if($mode == "list") {

		
		
		if($gamesrv_solo) {
			unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�...<BR>������ ������ SQL������ �ش�Ǵ� ������ �����Ҽ��� ���ų� ��Ʈ��ȣ���� �޶� ������ ���Ҽ��� �ֽ��ϴ�..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�..<BR>�� ������ �����ͺ��̽��� ������ ������ ���°���̰ų� �߸��� �����ͺ��̽�(DB)�̸����� ���� ������ �Ұ������� ����Դϴ�..");
					}
			}
		}
		else {
			unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�...<BR>������ ������ SQL������ �ش�Ǵ� ������ �����Ҽ��� ���ų� ��Ʈ��ȣ���� �޶� ������ ���Ҽ��� �ֽ��ϴ�..");
					}
				}
				$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�..<BR>�� ������ �����ͺ��̽��� ������ ������ ���°���̰ų� �߸��� �����ͺ��̽�(DB)�̸����� ���� ������ �Ұ������� ����Դϴ�..");
					}
		}	
		$pagewlist = 20;
		$pagenum = 10;
		$s_num = intval(($page*$pagewlist)-$pagewlist); // DB���� �ۻ̾ƿ� ���ۼ�
		$currb = ceil($page/$pagenum);
		$sp_num = ($currb-1)*$pagenum+1; // ����������Ʈ ���ۼ���
		
		if(isblanks($search)) {
		
		
		$result = @mysql_query("select * from chatlog  order by `time` desc limit $s_num,$pagewlist");
		$results = @mysql_query("select * from chatlog  order by `time` desc");
		
	
	}
	else {
		$search = urldecode($search);
		
		$result = @mysql_query("select * from chatlog  where message like '%$search%' order by `time` desc limit $s_num,$pagewlist");
		$results = @mysql_query("select * from chatlog  where message like '%$search%'  order by `time` desc");
	}
	
		$total = @mysql_num_rows($results); // �� �ۼ�
		
		$total_page = @ceil($total / $pagewlist); // �ѱۼ��� �������� ǥ���� �� ������ ������ �������� ������� �ݿø� �Ͽ� �� ���������� ����

		if(!$total_page) $total_page = 1;
	
		$pagenum2 = $sp_num+($pagenum-1);
		for($i=$sp_num;$i <= $pagenum2;$i++) {

		if($i == $page) {
			if(!$a_pagelist) $a_pagelist = "<span class='nowpage'>$i</span>&nbsp;";
			else $a_pagelist .= "<span class='nowpage'>$i</span>&nbsp;";
		}
		else {
		$a_pagelist .="<a href='$PHP_SELF?mode=$mode&search=$search&page=$i' class='pagelist'>$i</a>&nbsp;";
		}
		if($i >= $total_page) break;
		
	}

		$totalb = ceil($total_page/$pagenum);
		if($totalb > $currb) { // ǥ���� �������� 10���� �Ѿ��ٸ�.. ( ... ) ǥ�ø� �־�� 
			$gopage = $sp_num+$pagenum;
			$a_nextp ="<a href='$PHP_SELF?id=$id&search=$search&page=$gopage&sep=$sep&sc=$sc&sn=$sn&ss=$ss&category=$category' class='pagelist'>";
			
		}
		else $a_nextp ="<!-- �糪���� //-->";
		
		
		if($currb > 1) { // ���� �������� 10������ ��������� �������� ���� �ֵ��� ( ... ) �־��
			$gopage = $sp_num-$pagenum;
			$a_prevp ="<a href='$PHP_SELF?mode=$mode&search=$search&page=$gopage' class='pagelist'>";
		}
		else $a_prevp = "<!-- �糪���� //-->";
	
		if($page != $total_page) $a_lastpage ="<a href='$PHP_SELF?mode=$mode&search=$search&page=$total_page' class='pagelist'>";
		else $a_lastpage = "";

		if($page != 1) $a_firstpage ="<a href='$PHP_SELF?mode=$mode&search=$search&page=1' class='pagelist'>";
		else $a_firstpage = "";

			 unset($a_number);
    	$a_number = intval($total - ($page - 1) * $pagewlist); //���� �۹�ȣ (�ѱۼ� - (���� ������ - 1) * ���������� ǥ���� �ۼ�)
	
	echo "<form name='schlogfrm' method=post action='$PHP_SELF'>";
		include $skin_dir."/list_head.php";
		
	  if($total > 0) {	
		while($a_data = @mysql_fetch_array($result)) {
	
	
		$tmptime = explode(" ",$a_data["time"]);
		
		$tmptime2 = explode("-",$tmptime[0]);
		$tmptime3 = explode(":",$tmptime[1]);
		
		$years = $tmptime2[0]; //��
		$mons = $tmptime2[1]; // ��
		$days = $tmptime2[2]; //��
		$hours = $tmptime3[0]; //��
		$mins = $tmptime3[1]; // ��
		$secs = $tmptime3[2]; // ��
		
		$types = $a_data[type];
		
		$charsql = @mysql_fetch_array(@mysql_query("select * from `char` where char_id='$a_data[src_charid]'"));
		
		$srcname = $charsql[name];
		
		$mapname = $a_data[src_map];
		$mapx = $a_data[src_map_x];
		$mapy = $a_data[src_map_y];
		$message = strip_tags($a_data[message]);
		$message = stripslashes($message);
		$dst_name = $a_data[dst_charname];
		
		if(!$$mapname) $maps = $mapname;
		else $maps = $$mapname;
		
		if($types == "W") $cans = "�ӻ迴";
		else $cans = "�����Ͽ�";
		
		if($types == "O") $memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='#9900FF'> ".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."�п�</font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		else if($types == "W") $memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='red'>$dst_name</font><font color='#333333'>�Բ� </font><font color='#9900FF'>".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."�� ".$secs."�ʿ� </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		else if($types == "P") {
			$ptdata = @mysql_fetch_array(@mysql_query("select * from party where party_id='$a_data[type_id];"));
			$partys = $ptdata[name];
			
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='red'>$partys</font><font color='#333333'>�� </font><font color='#9900FF'>".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."��".$secs."�ʿ� </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else if($types == "G") {
			$gtdata = @mysql_fetch_array(@mysql_query("select * from guild where guild_id='$a_data[type_id];"));
			$guilds = $gtdata[name];
			
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='red'>$guilds</font><font color='#333333'>�� </font><font color='#9900FF'>".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."��".$secs."�ʿ� </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else if($types == "M") {
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='#9900FF'>".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."��".$secs."�ʿ� </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else {
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>�Բ��� </font><font color='#9900FF'>".$years."�� ".$mons."�� ".$days."�� ".$hours."�� ".$mins."��".$secs."�ʿ� </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>����</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(��)�� &quot;$message&quot; ��� ".$cans."���ϴ�.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}			
		
	include $skin_dir."/list_main.php";
	
}
}
else include $skin_dir."/list_nodata.php";

$a_list = "<a href='$PHP_SELF?mode=list&page=$page&search='>";
$a_delete = "$PHP_SELF?mode=alldelete";

include $skin_dir."/list_foot.php";
echo "</form>";

}
if($mode == "delete") {
	
		$no = $_GET["no"];
		if(!eregi(getenv("HTTP_HOST"),getenv("HTTP_REFERER"))) {
				msgbox("�������� ������ �ƴմϴ�...");
		}
	unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�...<BR>������ ������ SQL������ �ش�Ǵ� ������ �����Ҽ��� ���ų� ��Ʈ��ȣ���� �޶� ������ ���Ҽ��� �ֽ��ϴ�..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�..<BR>�� ������ �����ͺ��̽��� ������ ������ ���°���̰ų� �߸��� �����ͺ��̽�(DB)�̸����� ���� ������ �Ұ������� ����Դϴ�..");
					}
			}	
		$result = @mysql_query("delete from chatlog where id='$no'");
		
		if(!$result) {
			$errsql = addslashes(mysql_error());
			msgbox("�ش� ����� �����ϴ��� ������ �߻��Ǿ����ϴ�..<BR>��������: $errsql");
		}
		
			echo"<meta http-equiv='refresh' content='0; url=$PHP_SELF?mode=list'>";
}

if($mode == "alldelete") {
	
		$no = $_GET["no"];
		if(!eregi(getenv("HTTP_HOST"),getenv("HTTP_REFERER"))) {
				msgbox("�������� ������ �ƴմϴ�...");
		}
		
	unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�...<BR>������ ������ SQL������ �ش�Ǵ� ������ �����Ҽ��� ���ų� ��Ʈ��ȣ���� �޶� ������ ���Ҽ��� �ֽ��ϴ�..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL������ ������ ������ �߻��Ǿ����ϴ�..<BR>�� ������ �����ͺ��̽��� ������ ������ ���°���̰ų� �߸��� �����ͺ��̽�(DB)�̸����� ���� ������ �Ұ������� ����Դϴ�..");
					}
			}	
		$result = @mysql_query("delete from chatlog");
		
		if(!$result) {
			$errsql = addslashes(mysql_error());
			msgbox("�ش� ����� �����ϴ��� ������ �߻��Ǿ����ϴ�..<BR>��������: $errsql");
		}
		
			echo"<meta http-equiv='refresh' content='0; url=$PHP_SELF?mode=list'>";
}
	
?>