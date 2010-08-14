<?
include "setup.php";

$mode = $_GET["mode"];
$adid = $_POST["admin_id"];
$adpw = $_POST["admin_pw"];
if(!$mode) $mode = "list";
if($adid && !eregi(getenv("HTTP_HOST"),getenv("HTTP_REFERER"))) {
	msgbox("정상적인 접근이 아닙니다...");
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
		msgbox("관리자만 입장가능합니다...");
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
        <td width='275' style='border-top-width:0; border-right-width:0; border-bottom-width:3; border-left-width:0; border-top-color:black; border-right-color:black; border-bottom-color:rgb(204,231,255); border-left-color:black; border-top-style:none; border-right-style:none; border-bottom-style:solid; border-left-style:none;' bgcolor='white'><span style='font-size:9pt;'><font color='#999999'>메시지</font></span></td>
    </tr>
    <tr>
        <td width='275' style='border-top-width:1; border-right-width:0; border-bottom-width:1; border-left-width:0; border-top-color:rgb(153,153,153); border-right-color:black; border-bottom-color:rgb(153,153,153); border-left-color:black; border-top-style:dotted; border-right-style:none; border-bottom-style:dotted; border-left-style:none;' height='39'>
            <p align='center'><span style='font-size:9pt;'><font color='#333333'><br>$msg</font></span><br><br><input type='button' name='okbak' value='뒤로' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(165,180,224); border-style:solid;' onclick=\"javascript:$surl;\"></p>
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
	if($zboard_use && (!$_zb_path  || $_zb_path == "경로")) return false;
	if($lunaboard_use && (!$lunapath  || $lunapath == "경로")) return false;
	
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
    		alert('관리자 아이디를 입력하세요..')
    		form.admin_id.focus();
    		return false;
    	}
    	if(!form.admin_pw.value) {
    		alert('관리자 비밀번호를 입력하세요..')
    		form.admin_pw.focus();
    		return false;
    	}
    }
    //-->
    </script>
    		
    <form name='loginfrms' method=post action='$PHP_SELF?mode=list' onsubmit='return chkfrmadlogin(this);'>
<table width='315' cellspacing='0' style='border-collapse:collapse;' align='center'>
    <tr>
        <td width='313' style='border-top-width:0; border-right-width:0; border-bottom-width:3; border-left-width:0; border-top-color:black; border-right-color:black; border-bottom-color:rgb(204,231,255); border-left-color:black; border-top-style:none; border-right-style:none; border-bottom-style:solid; border-left-style:none;' bgcolor='white' colspan='3'><span style='font-size:9pt;'><font color='#666666'><b>관리자 로그인</b></font></span></td>
    </tr>
    <tr>
        <td width='313' style='border-top-width:1; border-right-width:0; border-bottom-width:1; border-left-width:0; border-top-color:rgb(153,153,153); border-right-color:black; border-bottom-color:rgb(153,153,153); border-left-color:black; border-top-style:dotted; border-right-style:none; border-bottom-style:dotted; border-left-style:none;' height='39' colspan='3'>

            <table width='311' cellspacing='0' style='border-collapse:collapse;'>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>아이디</font></span></td>
                    <td width='223' style='border-width:0; border-color:black; border-style:none;'>

                            <p><span style='font-size:9pt;'><font color='#666666'><input type='text' name='admin_id' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(153,153,153); border-style:dotted;'></font></span></p>

</td>
                </tr>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>비밀번호</font></span></td>
                        <td width='223' style='border-width:0; border-color:black; border-style:none;'>                            <p><span style='font-size:9pt;'><font color='#666666'><input type='password' name='admin_pw' style='font-size:9pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(153,153,153); border-style:dotted;' size='22'></font></span></p>
</td>
                </tr>
                <tr>
                    <td width='84' style='border-width:0; border-color:black; border-style:none;'><span style='font-size:9pt;'><font color='#999999'>&nbsp;</font></span></td>
                        <td width='223' style='border-width:0; border-color:black; border-style:none;'>                            <p align='right'><span style='font-size:9pt;'><input type='submit' name='loginok' value='로그인' style='font-size:8pt; color:rgb(51,51,51); background-color:white; border-width:1; border-color:rgb(165,180,224); border-style:solid;'></span></p>
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
	
	// 빈문자열인경우 True 반환 빈문자열이 아닌경우 False 를 반환한다.
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
						msgbox("SQL서버에 접속중 오류가 발생되었습니다...<BR>오류의 원인은 SQL정보에 해당되는 서버에 접속할수가 없거나 포트번호등이 달라서 접속이 못할수도 있습니다..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL서버에 접속중 오류가 발생되었습니다..<BR>이 오류는 데이터베이스에 접속할 권한이 없는경우이거나 잘못된 데이터베이스(DB)이름으로 인해 접속이 불가능해진 경우입니다..");
					}
			}
		}
		else {
			unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL서버에 접속중 오류가 발생되었습니다...<BR>오류의 원인은 SQL정보에 해당되는 서버에 접속할수가 없거나 포트번호등이 달라서 접속이 못할수도 있습니다..");
					}
				}
				$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL서버에 접속중 오류가 발생되었습니다..<BR>이 오류는 데이터베이스에 접속할 권한이 없는경우이거나 잘못된 데이터베이스(DB)이름으로 인해 접속이 불가능해진 경우입니다..");
					}
		}	
		$pagewlist = 20;
		$pagenum = 10;
		$s_num = intval(($page*$pagewlist)-$pagewlist); // DB에서 글뽑아올 시작수
		$currb = ceil($page/$pagenum);
		$sp_num = ($currb-1)*$pagenum+1; // 페이지리스트 시작숫자
		
		if(isblanks($search)) {
		
		
		$result = @mysql_query("select * from chatlog  order by `time` desc limit $s_num,$pagewlist");
		$results = @mysql_query("select * from chatlog  order by `time` desc");
		
	
	}
	else {
		$search = urldecode($search);
		
		$result = @mysql_query("select * from chatlog  where message like '%$search%' order by `time` desc limit $s_num,$pagewlist");
		$results = @mysql_query("select * from chatlog  where message like '%$search%'  order by `time` desc");
	}
	
		$total = @mysql_num_rows($results); // 총 글수
		
		$total_page = @ceil($total / $pagewlist); // 총글수를 페이지에 표시할 글 갯수로 나누어 나머지가 있을경우 반올림 하여 총 페이지수를 구함

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
		if($totalb > $currb) { // 표시할 페이지가 10개가 넘었다면.. ( ... ) 표시를 넣어둠 
			$gopage = $sp_num+$pagenum;
			$a_nextp ="<a href='$PHP_SELF?id=$id&search=$search&page=$gopage&sep=$sep&sc=$sc&sn=$sn&ss=$ss&category=$category' class='pagelist'>";
			
		}
		else $a_nextp ="<!-- 루나보드 //-->";
		
		
		if($currb > 1) { // 현재 페이지가 10페이지 넘은경우라면 이전으로 갈수 있도록 ( ... ) 넣어둠
			$gopage = $sp_num-$pagenum;
			$a_prevp ="<a href='$PHP_SELF?mode=$mode&search=$search&page=$gopage' class='pagelist'>";
		}
		else $a_prevp = "<!-- 루나보드 //-->";
	
		if($page != $total_page) $a_lastpage ="<a href='$PHP_SELF?mode=$mode&search=$search&page=$total_page' class='pagelist'>";
		else $a_lastpage = "";

		if($page != 1) $a_firstpage ="<a href='$PHP_SELF?mode=$mode&search=$search&page=1' class='pagelist'>";
		else $a_firstpage = "";

			 unset($a_number);
    	$a_number = intval($total - ($page - 1) * $pagewlist); //임의 글번호 (총글수 - (현재 페이지 - 1) * 한페이지에 표시할 글수)
	
	echo "<form name='schlogfrm' method=post action='$PHP_SELF'>";
		include $skin_dir."/list_head.php";
		
	  if($total > 0) {	
		while($a_data = @mysql_fetch_array($result)) {
	
	
		$tmptime = explode(" ",$a_data["time"]);
		
		$tmptime2 = explode("-",$tmptime[0]);
		$tmptime3 = explode(":",$tmptime[1]);
		
		$years = $tmptime2[0]; //년
		$mons = $tmptime2[1]; // 월
		$days = $tmptime2[2]; //일
		$hours = $tmptime3[0]; //시
		$mins = $tmptime3[1]; // 분
		$secs = $tmptime3[2]; // 초
		
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
		
		if($types == "W") $cans = "속삭였";
		else $cans = "말을하였";
		
		if($types == "O") $memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='#9900FF'> ".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분에</font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		else if($types == "W") $memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='red'>$dst_name</font><font color='#333333'>님께 </font><font color='#9900FF'>".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분 ".$secs."초에 </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		else if($types == "P") {
			$ptdata = @mysql_fetch_array(@mysql_query("select * from party where party_id='$a_data[type_id];"));
			$partys = $ptdata[name];
			
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='red'>$partys</font><font color='#333333'>의 </font><font color='#9900FF'>".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분".$secs."초에 </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else if($types == "G") {
			$gtdata = @mysql_fetch_array(@mysql_query("select * from guild where guild_id='$a_data[type_id];"));
			$guilds = $gtdata[name];
			
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='red'>$guilds</font><font color='#333333'>의 </font><font color='#9900FF'>".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분".$secs."초에 </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else if($types == "M") {
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='#9900FF'>".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분".$secs."초에 </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
		}
		else {
			$memo = "<span style='font-size:9pt;'><font color='blue'>$srcname</font><font color='#333333'>님께서 </font><font color='#9900FF'>".$years."년 ".$mons."월 ".$days."일 ".$hours."시 ".$mins."분".$secs."초에 </font><font color='#1E8484'>$maps $mapx , $mapy </font><font color='#333333'>에서</font><BR><font color='#006600'>{$$types}</font><font color='#333333'>(으)로 &quot;$message&quot; 라고 ".$cans."습니다.... </font></span><a href='$PHP_SELF?mode=delete&no=$a_data[id]'><img align='absmiddle' src='$skin_dir/image/delete.gif' width='40' height='17' border='0'></a>";
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
				msgbox("정상적인 접근이 아닙니다...");
		}
	unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL서버에 접속중 오류가 발생되었습니다...<BR>오류의 원인은 SQL정보에 해당되는 서버에 접속할수가 없거나 포트번호등이 달라서 접속이 못할수도 있습니다..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL서버에 접속중 오류가 발생되었습니다..<BR>이 오류는 데이터베이스에 접속할 권한이 없는경우이거나 잘못된 데이터베이스(DB)이름으로 인해 접속이 불가능해진 경우입니다..");
					}
			}	
		$result = @mysql_query("delete from chatlog where id='$no'");
		
		if(!$result) {
			$errsql = addslashes(mysql_error());
			msgbox("해당 기록을 삭제하던중 오류가 발생되었습니다..<BR>오류내용: $errsql");
		}
		
			echo"<meta http-equiv='refresh' content='0; url=$PHP_SELF?mode=list'>";
}

if($mode == "alldelete") {
	
		$no = $_GET["no"];
		if(!eregi(getenv("HTTP_HOST"),getenv("HTTP_REFERER"))) {
				msgbox("정상적인 접근이 아닙니다...");
		}
		
	unset($connect);
				if(!$connect) {
					$connect = @mysql_connect($SQLHOST.":".$SQLPORT,$SQLID,$SQLPW);
					if(!$connect) {
						msgbox("SQL서버에 접속중 오류가 발생되었습니다...<BR>오류의 원인은 SQL정보에 해당되는 서버에 접속할수가 없거나 포트번호등이 달라서 접속이 못할수도 있습니다..");
					}
		
					$seldb = @mysql_select_db($SQLDB,$connect);
					
					if(!$seldb) {
							msgbox("SQL서버에 접속중 오류가 발생되었습니다..<BR>이 오류는 데이터베이스에 접속할 권한이 없는경우이거나 잘못된 데이터베이스(DB)이름으로 인해 접속이 불가능해진 경우입니다..");
					}
			}	
		$result = @mysql_query("delete from chatlog");
		
		if(!$result) {
			$errsql = addslashes(mysql_error());
			msgbox("해당 기록을 삭제하던중 오류가 발생되었습니다..<BR>오류내용: $errsql");
		}
		
			echo"<meta http-equiv='refresh' content='0; url=$PHP_SELF?mode=list'>";
}
	
?>