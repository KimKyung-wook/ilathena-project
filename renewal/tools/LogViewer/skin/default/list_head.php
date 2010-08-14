<script>
	<!--
	function alldelchk(dellink) {
		var form = document.all.schlogfrm;
		
		if(confirm("모든 기록을 정말로 삭제하시겠습니까?")) {
				form.action=dellink;
				form.submit();
			}
		}
//-->
</script>
<table width="517" cellspacing="0" style="border-collapse:collapse;" align="center">
    <tr>
        <td width="515" style="border-top-width:0; border-right-width:0; border-bottom-width:3; border-left-width:0; border-top-color:black; border-right-color:black; border-bottom-color:rgb(204,231,255); border-left-color:black; border-top-style:none; border-right-style:none; border-bottom-style:solid; border-left-style:none;" bgcolor="white" colspan="3"><span style="font-size:9pt;"><font color="#666666">전체로그: </font><font color="#CC3300"><?=$total?></font><font color="#666666">개 &nbsp;페이지: </font><font color="blue"><?=$page?></font><font color="#666666">/</font><font color="#CC3300"><?=$total_page?></font><font color="#666666">P</font></span></td>
    </tr>